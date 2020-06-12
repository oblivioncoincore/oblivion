// Copyright (c) 2019-2020 The Oblivion developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef OBLIVION_QT_MULTISIGADDRESSENTRY_H
#define OBLIVION_QT_MULTISIGADDRESSENTRY_H

#include <QFrame>

class WalletModel;
class PlatformStyle;

namespace Ui
{
    class MultisigAddressEntry;
}

class MultisigAddressEntry : public QFrame
{
    Q_OBJECT;

  public:
    explicit MultisigAddressEntry(QWidget *parent = 0);
    ~MultisigAddressEntry();
    void setModel(WalletModel *model);
    bool validate();
    QString getPubkey();

  public Q_SLOTS:
    void setRemoveEnabled(bool enabled);
    void clear();

  Q_SIGNALS:
    void removeEntry(MultisigAddressEntry *entry);

  private:
    Ui::MultisigAddressEntry *ui;
    WalletModel *model;
    const PlatformStyle *platformStyle;

  private Q_SLOTS:
    void on_pubkey_textChanged(const QString &pubkey);
    void on_pasteButton_clicked();
    void on_deleteButton_clicked();
    void on_address_textChanged(const QString &address);
    void on_addressBookButton_clicked();
};

#endif // OBLIVION_QT_MULTISIGADDRESSENTRY_H
