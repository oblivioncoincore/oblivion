// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <genesis.h>
#include <consensus/merkle.h>

#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <assert.h>
#include <memory>

#include <chainparamsseeds.h>
#include <arith_uint256.h>
#include <pthread.h>

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0;
        consensus.powLimit =            uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        consensus.nTargetTimespan = 24 * 60;  // 24 minutes
        consensus.nStakeTargetSpacing = 2 * 60; // 2-minute block spacing
        consensus.nTargetSpacingWorkMax = 12 * consensus.nStakeTargetSpacing; // 24-minutes
        consensus.nPowTargetSpacing = consensus.nStakeTargetSpacing;
        consensus.nStakeMinConfirmations = 240; // 240 * 2 minutes
        consensus.nModifierInterval = 10 * 60; // Modifier interval: time to elapse before new modifier is computed
        consensus.nCoinbaseMaturity = 100;
        consensus.nDgwPastBlocks = 30;

        // POW block consensus
        consensus.warmUpPOWBlock = 1440; // 2 days warmup
        consensus.nTotalPOWBlock = consensus.warmUpPOWBlock + 40000; // ~2 months + first day
        consensus.nStartMiningTime = 1591887600;

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00"); // 350000

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");  // 380000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xfb;
        pchMessageStart[1] = 0xf3;
        pchMessageStart[2] = 0xef;
        pchMessageStart[3] = 0xb4;
        vAlertPubKey = ParseHex("040797a85cafdf223783f0e31b7e3554b61f873efb0abaa6fa56632308ae2d7309a8afdf6c221a07f73fc288c7626f3cd97a6121241db0d60e0e375aaa87f4b9a2");
        nDefaultPort = 6388;

        genesis = CreateGenesisBlock(1591772283, 1100841907, 0x1e0fffff, 1, 0);
//        MineGenesisBlock(genesis, consensus.powLimit);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("296a3b214898cde66b121ec84948cd887bef3cfb7ad17b48d3b28d3cf2dce921"));
        assert(genesis.hashMerkleRoot == uint256S("45c6b6cbf96ff4c9ca7beaf5f9ae89782e299abb98d3ebafcd9cc11b67397410"));

        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("35.223.95.142");
        vSeeds.emplace_back("35.224.88.175");
        vSeeds.emplace_back("35.184.223.75");
        vSeeds.emplace_back("35.226.209.23");

        base58Prefixes[PUBKEY_ADDRESS] = {0x42}; // T
        base58Prefixes[SCRIPT_ADDRESS] = {0x3c};
        base58Prefixes[SECRET_KEY]     = {0x6c};

        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E}; // xpub
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4}; // xprv

        // human readable prefix to bench32 address
        bech32_hrp = "ts";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        nMiningRequiresPeers = 3;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
                {0, uint256S("843a90aa80dd935bd6a567337d53e391f9e7729d18ff7138c9a1cb6f7148a14e")},
            }
        };

        chainTxData = ChainTxData{
                // Data as of block ???
                0, // * UNIX timestamp of last known number of transactions
                0,     // * total number of transactions between genesis and that timestamp
                //   (the tx=... number in the SetBestChain debug.log lines)
                0 // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 24 * 60;  // 24 minutes
        consensus.nStakeTargetSpacing = 2 * 60;  // 2-minute block spacing
        consensus.nTargetSpacingWorkMax = 12 * consensus.nStakeTargetSpacing; // 24-minute
        consensus.nPowTargetSpacing = consensus.nStakeTargetSpacing;
        consensus.nStakeMinConfirmations = 20; // test net min age is 20 confirms
        consensus.nModifierInterval = 10 * 60; // Modifier interval: time to elapse before new modifier is computed
        consensus.nCoinbaseMaturity = 10; // 50 confirmations
        consensus.nDgwPastBlocks = 30;

        // POW block consensus
        consensus.warmUpPOWBlock = 200; // 1 day + first day
        consensus.nTotalPOWBlock = consensus.warmUpPOWBlock + 200; // 1 day + first day

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nStartMiningTime = 1581441000;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xcb;
        pchMessageStart[1] = 0xf2;
        pchMessageStart[2] = 0xc3;
        pchMessageStart[3] = 0xef;
        nDefaultPort = 6366;
        vAlertPubKey = ParseHex("04173a381c9a7c0bf2e8f5dd3c71d059b025b6fee42b92224af842bd40ba1c995d4e2e79d2fda539471ffb2cec48f45557c3f34d0269da3b6ae31eb7f48039b719");

        genesis = CreateGenesisBlock(1581428205, 500000675, 0x1f00ffff, 1, 0);
//        MineGenesisBlock(genesis, consensus.powLimit);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("c4fa9f6a00492ad14854ae03bf02dfd2aaff667349d806092b13d829674e5a06"));
        assert(genesis.hashMerkleRoot == uint256S("74894a4df74f4ef7657144f1372c28105679317795563b932a6948ed41a8bb5d"));

        vFixedSeeds.clear();
        vSeeds.clear();

        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("35.223.95.142");
        vSeeds.emplace_back("35.224.88.175");
        vSeeds.emplace_back("35.184.223.75");
        vSeeds.emplace_back("35.226.209.23");

        base58Prefixes[PUBKEY_ADDRESS] = {0x80}; // t
        base58Prefixes[SCRIPT_ADDRESS] = {0x7a};
        base58Prefixes[SECRET_KEY]     = {0x2e};
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF}; // tpub
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94}; // tprv

        // human readable prefix to bench32 address
        bech32_hrp = "tst";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        nMiningRequiresPeers = 2;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = {
                {
                        {0, uint256S("c4fa9f6a00492ad14854ae03bf02dfd2aaff667349d806092b13d829674e5a06")},
            }
        };

        chainTxData = ChainTxData{
                0,
                0,
                0
        };

    }
};

/**
 * Regression test
 */

class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.BIP16Height = 0; // always enforce P2SH BIP16 on regtest
        consensus.BIP34Height = 0; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.powLimit =            uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~arith_uint256(0) >> 28;
        consensus.nTargetTimespan = 3 * 24 * 60 * 60; // two weeks
        consensus.nStakeTargetSpacing = 2 * 60; // 2-minute block spacing
        consensus.nTargetSpacingWorkMax = 12 * consensus.nStakeTargetSpacing; // 2-hour
        consensus.nPowTargetSpacing = consensus.nStakeTargetSpacing;
        consensus.nStakeMinConfirmations = 2; // test net min age is 1 day
        consensus.nModifierInterval = 2 * 60; // Modifier interval: time to elapse before new modifier is computed
        consensus.nCoinbaseMaturity = 2;
        consensus.nDgwPastBlocks = 3;

        // POW block consensus
        consensus.warmUpPOWBlock = 1440; // 2 days warmup
        consensus.nTotalPOWBlock = consensus.warmUpPOWBlock + 40000; // ~2 months + first day

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nStartMiningTime = 1581501960;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xcb;
        pchMessageStart[1] = 0xf2;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xef;
        nDefaultPort = 6333;
        vAlertPubKey = ParseHex("04173a381c9a7c0bf2e8f5dd3c71d059b025b6fee42b92224af842bd40ba1c995d4e2e79d2fda539471ffb2cec48f45557c3f34d0269da3b6ae31eb7f48039b719");

        genesis = CreateGenesisBlock(1565753832, 300011609, 0x1f00ffff, 1, 0);
//        MineGenesisBlock(genesis, consensus.powLimit);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("65f8e03c5b49085a78e5422d637aaf02e7d8fc7ce93be8f563e4032854731e41"));
        assert(genesis.hashMerkleRoot == uint256S("7a12d0f54abd64da88bbe04907daa662a18ee900e886e1e98f78b934f9ca18e7"));


        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        checkpointData = {
            {
                {0, uint256S("65f8e03c5b49085a78e5422d637aaf02e7d8fc7ce93be8f563e4032854731e41")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = {0x80}; // t
        base58Prefixes[SCRIPT_ADDRESS] = {0x7a};
        base58Prefixes[SECRET_KEY]     = {0x2e};
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF}; // tpub
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94}; // tprv

        bech32_hrp = "tsrt";

        nMiningRequiresPeers = 0;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
    }
};

static std::unique_ptr <CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr <CChainParams> CreateChainParams(const std::string &chain) {
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string &network) {
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}
