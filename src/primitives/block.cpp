// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>
#include <util/strencodings.h>
#include <crypto/common.h>
#include <pubkey.h>
#include <sync.h>

// yespower
#include <crypto/yespower/yespower.h>
#include <streams.h>
#include <version.h>

// yespower exit()
#include <stdlib.h>

// Used to serialize the header without signature
// Workaround due to removing serialization templates in Bitcoin Core 0.18
class CBlockHeaderSign
{
public:
    CBlockHeaderSign(const CBlockHeaderUncached& header)
    {
        fHasProofOfDelegation = header.HasProofOfDelegation();
        nVersion = header.nVersion;
        hashPrevBlock = header.hashPrevBlock;
        hashMerkleRoot = header.hashMerkleRoot;
        nTime = header.nTime;
        nBits = header.nBits;
        nNonce = header.nNonce;
        hashStateRoot = header.hashStateRoot;
        hashUTXORoot = header.hashUTXORoot;
        prevoutStake = header.prevoutStake;
        vchBlockDlgt = header.GetProofOfDelegation();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(this->nVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
        READWRITE(nNonce);
        READWRITE(hashStateRoot);
        READWRITE(hashUTXORoot);
        READWRITE(prevoutStake);
        if(fHasProofOfDelegation)
        {
            READWRITE(vchBlockDlgt);
        }
    }

private:
    bool fHasProofOfDelegation;

    // header without signature
    int32_t nVersion;
    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint32_t nTime;
    uint32_t nBits;
    uint32_t nNonce;
    uint256 hashStateRoot;
    uint256 hashUTXORoot;
    COutPoint prevoutStake;
    std::vector<unsigned char> vchBlockDlgt;
};

uint256 CBlockHeaderUncached::GetHash() const
{
    return SerializeHash(*this);
}

uint256 CBlockHeaderUncached::GetHashWithoutSign() const
{
    return SerializeHash(CBlockHeaderSign(*this), SER_GETHASH);
}

uint256 CBlockHeaderUncached::GetWorkHash() const
{
    static const yespower_params_t yespower_1_0_config = {
        .version = YESPOWER_1_0,
        .N = 2048,
        .r = 32,
        .pers = (const uint8_t *)"The gods had gone away, and the ritual of the religion continued senselessly, uselessly.",
        .perslen = 88
    };

    uint256 hash;
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << *this;
    if (yespower_tls((const uint8_t *)&ss[0], ss.size(), &yespower_1_0_config, (yespower_binary_t *)&hash)) {
        fprintf(stderr, "Error: CBlockHeaderUncached::GetPoWHash(): failed to compute PoW hash (out of memory?)\n");
        exit(1);
    }

    return hash;
}

uint256 CBlockHeader::GetWorkHashCached() const
{
    uint256 indexHash = GetHash();
    LOCK(cacheLock);
    if (cacheInit) {
        if (indexHash != cacheIndexHash) {
            fprintf(stderr, "Error: CBlockHeader: block hash changed unexpectedly\n");
            exit(1);
        }
    } else {
        cacheWorkHash = IsProofOfWork() ? GetWorkHash() : indexHash;
        cacheIndexHash = indexHash;
        cacheInit = true;
    }
    return cacheWorkHash;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, hashStateRoot=%s, hashUTXORoot=%s, blockSig=%s, proof=%s, prevoutStake=%s, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        hashStateRoot.ToString(), // eqpay
        hashUTXORoot.ToString(), // eqpay
        HexStr(vchBlockSigDlgt),
        IsProofOfStake() ? "PoS" : "PoW",
        prevoutStake.ToString(),
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}

std::vector<unsigned char> CBlockHeaderUncached::GetBlockSignature() const
{
    if(vchBlockSigDlgt.size() < 2 * CPubKey::COMPACT_SIGNATURE_SIZE)
    {
        return vchBlockSigDlgt;
    }

    return std::vector<unsigned char>(vchBlockSigDlgt.begin(), vchBlockSigDlgt.end() - CPubKey::COMPACT_SIGNATURE_SIZE );
}

std::vector<unsigned char> CBlockHeaderUncached::GetProofOfDelegation() const
{
    if(vchBlockSigDlgt.size() < 2 * CPubKey::COMPACT_SIGNATURE_SIZE)
    {
        return std::vector<unsigned char>();
    }

    return std::vector<unsigned char>(vchBlockSigDlgt.begin() + vchBlockSigDlgt.size() - CPubKey::COMPACT_SIGNATURE_SIZE, vchBlockSigDlgt.end());

}

bool CBlockHeaderUncached::HasProofOfDelegation() const
{
    return vchBlockSigDlgt.size() >= 2 * CPubKey::COMPACT_SIGNATURE_SIZE;
}

void CBlockHeaderUncached::SetBlockSignature(const std::vector<unsigned char> &vchSign)
{
    if(HasProofOfDelegation())
    {
        std::vector<unsigned char> vchPoD = GetProofOfDelegation();
        vchBlockSigDlgt = vchSign;
        vchBlockSigDlgt.insert(vchBlockSigDlgt.end(), vchPoD.begin(), vchPoD.end());
    }
    else
    {
        vchBlockSigDlgt = vchSign;
    }
}

void CBlockHeaderUncached::SetProofOfDelegation(const std::vector<unsigned char> &vchPoD)
{
    std::vector<unsigned char> vchSign = GetBlockSignature();
    if(vchSign.size() != CPubKey::COMPACT_SIGNATURE_SIZE)
    {
        vchSign.resize(CPubKey::COMPACT_SIGNATURE_SIZE, 0);
    }
    vchBlockSigDlgt = vchSign;
    vchBlockSigDlgt.insert(vchBlockSigDlgt.end(), vchPoD.begin(), vchPoD.end());
}
