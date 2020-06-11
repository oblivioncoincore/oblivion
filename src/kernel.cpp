// Copyright (c) 2012-2019 The Oblivion developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel.h>
#include <chainparams.h>
#include <util.h>
#include <validation.h>
#include <streams.h>
#include <timedata.h>
#include <bignum.h>
#include <txdb.h>
#include <consensus/validation.h>
#include <random.h>
#include <script/interpreter.h>

#include <boost/assign/list_of.hpp>

using namespace std;

// Stake Modifier (hash modifier of proof-of-stake):
// The purpose of stake modifier is to prevent a txout (coin) owner from
// computing future proof-of-stake generated by this txout at the time
// of transaction confirmation. To meet kernel protocol, the txout
// must hash with a future stake modifier to generate the proof.
uint256 ComputeStakeModifier(const CBlockIndex* pindexPrev, const uint256& kernel)
{
    if (!pindexPrev)
        return uint256();  // genesis block's modifier is 0

    CDataStream ss(SER_GETHASH, 0);
    ss << kernel << pindexPrev->bnStakeModifier;
    return Hash(ss.begin(), ss.end());
}

// Oblivion kernel protocol
// coinstake must meet hash target according to the protocol:
// kernel (input 0) must meet the formula
//     hash(nStakeModifier + txPrev.block.nTime + txPrev.nTime + txPrev.vout.hash + txPrev.vout.n + nTime) < bnTarget * nWeight
// this ensures that the chance of getting a coinstake is proportional to the
// amount of coins one owns.
// The reason this hash is chosen is the following:
//   nStakeModifier: scrambles computation to make it very difficult to precompute
//                   future proof-of-stake
//   txPrev.block.nTime: prevent nodes from guessing a good timestamp to
//                       generate transaction for future advantage
//   txPrev.nTime: slightly scrambles computation
//   txPrev.vout.hash: hash of txPrev, to reduce the chance of nodes
//                     generating coinstake at the same time
//   txPrev.vout.n: output number of txPrev, to reduce the chance of nodes
//                  generating coinstake at the same time
//   nTime: current timestamp
//   block/tx hash should not be used here as they can be generated in vast
//   quantities so as to generate blocks faster, degrading the system back into
//   a proof-of-work situation.
//
bool CheckStakeKernelHash(unsigned int nBits, CBlockIndex* pindexPrev, const CBlockHeader& blockFrom, const CTransactionRef& txPrev, const COutPoint& prevout, unsigned int nTimeTx, uint256& hashProofOfStake, uint256& targetProofOfStake, bool fPrintProofOfStake)
{
    unsigned int nTimeBlockFrom = blockFrom.GetBlockTime();
    if (nTimeTx < txPrev->nTime)  // Transaction timestamp violation
        return error("CheckStakeKernelHash() : nTime violation");

    // Base target
    CBigNum bnTarget;
    bnTarget.SetCompact(nBits);

    // Weighted target
    int64_t nValueIn = txPrev->vout[prevout.n].nValue;
    CBigNum bnWeight = CBigNum(nValueIn);
    bnTarget *= bnWeight;

    targetProofOfStake = bnTarget.getuint256();

    uint256 bnStakeModifier = pindexPrev->bnStakeModifier;

    // Calculate hash
    CDataStream ss(SER_GETHASH, 0);
    ss << bnStakeModifier;
    ss << txPrev->nTime << prevout.hash << prevout.n << nTimeTx;
    hashProofOfStake = Hash(ss.begin(), ss.end());

    if (fPrintProofOfStake)
    {
        LogPrint(BCLog::ALL, "check modifier%s nTimeBlockFrom=%u nTimeTxPrev=%u nPrevout=%u nTimeTx=%u hashProof=%s\n",
                  bnStakeModifier.ToString(),
                  nTimeBlockFrom, txPrev->nTime, prevout.n, nTimeTx,
                  hashProofOfStake.ToString());
    }

    // Now check if proof-of-stake hash meets target protocol
    if (CBigNum(hashProofOfStake) > bnTarget) {
        return false;
    }

    if (!fPrintProofOfStake)
    {
        LogPrint(BCLog::ALL, "pass modifier=%s nTimeBlockFrom=%u nTimeTxPrev=%u nPrevout=%u nTimeTx=%u hashProof=%s\n",
                  bnStakeModifier.ToString(),
                  nTimeBlockFrom, txPrev->nTime, prevout.n, nTimeTx,
                  hashProofOfStake.ToString());
    }

    return true;
}

// Check kernel hash target and coinstake signature
bool CheckProofOfStake(CValidationState &state, CBlockIndex* pindexPrev, const CTransactionRef& tx, unsigned int nBits, uint256& hashProofOfStake, uint256& targetProofOfStake)
{
    if (!tx->IsCoinStake())
        return error("CheckProofOfStake() : called on non-coinstake %s", tx->GetHash().ToString());

    // Kernel (input 0) must match the stake hash target per coin age (nBits)
    const CTxIn& txin = tx->vin[0];

    // Transaction index is required to get to block header
    if (!fTxIndex)
        return error("CheckProofOfStake() : transaction index not available");

    // Get transaction index for the previous transaction
    CDiskTxPos postx;
    if (!pblocktree->ReadTxIndex(txin.prevout.hash, postx))
        return error("CheckProofOfStake() : tx index not found");  // tx index not found

    // Read txPrev and header of its block
    CBlockHeader header;
    CTransactionRef txPrev;
    CBlock blockKernel; // block containing stake kernel, GetTransaction should only fill the header.
    if (!GetTransaction(txin.prevout.hash, txPrev, Params().GetConsensus(), blockKernel)) {
        LogPrintf("ERROR: %s: prevout-not-in-chain\n", __func__);
        return error("prevout-not-in-chain");
    }
    int nDepth;
    if (IsConfirmedInNPrevBlocks(header.GetHash(), pindexPrev, Params().GetConsensus().nStakeMinConfirmations - 1, nDepth)) {
        return error("CheckProofOfStake() : tried to stake at depth %d", nDepth + 1);
    }

    if (txPrev->GetHash() != txin.prevout.hash)
        return error("%s() : txid mismatch in CheckProofOfStake()", __PRETTY_FUNCTION__);

    // Verify signature
    {
        int nIn = 0;
        const CTxOut& prevOut = txPrev->vout[tx->vin[nIn].prevout.n];
        TransactionSignatureChecker checker(&(*tx), nIn, prevOut.nValue, PrecomputedTransactionData(*tx));

        if (!VerifyScript(tx->vin[nIn].scriptSig, prevOut.scriptPubKey, &(tx->vin[nIn].scriptWitness), SCRIPT_VERIFY_P2SH, checker, nullptr))
            return state.DoS(100, false, REJECT_INVALID, "invalid-pos-script", false, strprintf("%s: VerifyScript failed on coinstake %s", __func__, tx->GetHash().ToString()));
    }

    if (!CheckStakeKernelHash(nBits, pindexPrev, header, txPrev, txin.prevout, tx->nTime, hashProofOfStake, targetProofOfStake, gArgs.GetBoolArg("-debug", false)))
        return state.DoS(1, error("CheckProofOfStake() : INFO: check kernel failed on coinstake %s, hashProof=%s", tx->GetHash().ToString(), hashProofOfStake.ToString())); // may occur during initial download or if behind on block chain sync

    return true;
}

// Check whether the coinstake timestamp meets protocol
bool CheckCoinStakeTimestamp(int64_t nTimeBlock, int64_t nTimeTx)
{
    return (nTimeBlock == nTimeTx);
}

// Used only when staking, not during validation
bool CheckKernel(unsigned int nBits, CBlockIndex *pindexPrev, const CBlockHeader& header, const CTransactionRef& txPrev, const COutPoint &prevoutStake, unsigned int nTime)
{
    uint256 hashProofOfStake, targetProofOfStake;
    int nDepth;

    if (IsConfirmedInNPrevBlocks(header.GetHash(), pindexPrev, Params().GetConsensus().nStakeMinConfirmations - 1, nDepth)) {
        return false;
    }

    return CheckStakeKernelHash(nBits, pindexPrev, header, txPrev, prevoutStake, nTime, hashProofOfStake, targetProofOfStake);
}