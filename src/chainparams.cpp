// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <consensus/consensus.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <util/convert.h>
#include <versionbitsinfo.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

///////////////////////////////////////////// // qtum
#include <libdevcore/SHA3.h>
#include <libdevcore/RLP.h>
#include "arith_uint256.h"
/////////////////////////////////////////////

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 00 << 488804799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    genesis.hashStateRoot = uint256(h256Touint(dev::h256("e965ffd002cd6ad0e2dc402b8044de833e06b23127ea8c3d80aec91410771495"))); // qtum
    genesis.hashUTXORoot = uint256(h256Touint(dev::sha3(dev::rlp("")))); // qtum
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Sep 02, 2017 Bitcoin breaks $5,000 in latest price frenzy";
    const CScript genesisOutputScript = CScript() << ParseHex("040d61d8653448c98731ee5fffd303c15e71ec2057b77f11ab3601979728cdaff2d68afbba14e4fa0bc44f2072b0b23ef63717f8cdfbe58dcd33f32b6afe98741a") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = CBaseChainParams::MAIN;
        consensus.QIP6Height = 1;
        consensus.QIP7Height = 1;
        consensus.nOfflineStakeHeight = 1;
        consensus.nMuirGlacierHeight = 1;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 2000;
        consensus.nTargetSpacing = 64;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000"); // qtum

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x02caf7a26b995e5054462715a4d31e1a7ff220c53fead7c06de720ac54510433"); // 888000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf1;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0xa6;
        pchMessageStart[3] = 0xd3;
        nDefaultPort = 3888;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 8;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1504695029, 8026361, 0x1f00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000075aef83cf2853580f8ae8ce6f8c3096cfa21d98334d6e3f95e5582ed986c"));
        assert(genesis.hashMerkleRoot == uint256S("0xed34050eb5909ee535fcb07af292ea55f3d2f291187617b44d3282231405b96d"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,58);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,50);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "qc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        m_is_test_chain = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                { 0, uint256S("000075aef83cf2853580f8ae8ce6f8c3096cfa21d98334d6e3f95e5582ed986c")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 76b1e67fcff0fcfd078d499c65494cee4319f256da09f5fdefa574433f7d4e3c (height 709065)
            1602362976, // * UNIX timestamp of last known number of transactions
            4340534, // * total number of transactions between genesis and that timestamp
            //   (the tx=... number in the SetBestChain debug.log lines)
            0.02433574394826639 // * estimated number of transactions per second after that timestamp
        };

        consensus.nCoinbaseMaturity = 500;

        consensus.nCheckpointSpan = consensus.nCoinbaseMaturity;
        consensus.delegationsAddress = uint160(ParseHex("0000000000000000000000000000000000000086")); // Delegations contract for offline staking
        consensus.nStakeTimestampMask = 3;
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = CBaseChainParams::TESTNET;
        consensus.QIP6Height = 1;
        consensus.QIP7Height = 1;
        consensus.nOfflineStakeHeight = 1;
        consensus.nMuirGlacierHeight = 1;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 2000;
        consensus.nTargetSpacing = 64;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000"); // qtum

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x6bb6312088d81ca5484460b3466c66c01ff7d1cd4ef91e1dc9555a15b51d025d"); // 944000

        pchMessageStart[0] = 0x0d;
        pchMessageStart[1] = 0x22;
        pchMessageStart[2] = 0x15;
        pchMessageStart[3] = 0x06;
        nDefaultPort = 13888;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 4;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1504695029, 7349697, 0x1f00ffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000e803ee215c0684ca0d2f9220594d3f828617972aad66feb2ba51f5e14222"));
        assert(genesis.hashMerkleRoot == uint256S("0xed34050eb5909ee535fcb07af292ea55f3d2f291187617b44d3282231405b96d"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,120);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,110);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tq";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        m_is_test_chain = true;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {0, uint256S("0000e803ee215c0684ca0d2f9220594d3f828617972aad66feb2ba51f5e14222")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        consensus.nCoinbaseMaturity = 500;

        consensus.nCheckpointSpan = consensus.nCoinbaseMaturity;
        consensus.delegationsAddress = uint160(ParseHex("0000000000000000000000000000000000000086")); // Delegations contract for offline staking
        consensus.nStakeTimestampMask = 3;
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    explicit CRegTestParams(const ArgsManager& args) {
        strNetworkID =  CBaseChainParams::REGTEST;
        consensus.QIP6Height = 0;
        consensus.QIP7Height = 0;
        consensus.nOfflineStakeHeight = 1;
        consensus.nMuirGlacierHeight = 1;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 2000;
        consensus.nTargetSpacing = 64;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xfd;
        pchMessageStart[1] = 0xdd;
        pchMessageStart[2] = 0xc6;
        pchMessageStart[3] = 0xe1;
        nDefaultPort = 23888;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

        genesis = CreateGenesisBlock(1504695029, 17, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x665ed5b402ac0b44efc37d8926332994363e8a7278b7ee9a58fb972efadae943"));
        assert(genesis.hashMerkleRoot == uint256S("0xed34050eb5909ee535fcb07af292ea55f3d2f291187617b44d3282231405b96d"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        fMineBlocksOnDemand = true;
        m_is_test_chain = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("665ed5b402ac0b44efc37d8926332994363e8a7278b7ee9a58fb972efadae943")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        consensus.nCoinbaseMaturity = 500;

        consensus.nCheckpointSpan = consensus.nCoinbaseMaturity;
        consensus.delegationsAddress = uint160(ParseHex("0000000000000000000000000000000000000086")); // Delegations contract for offline staking
        consensus.nStakeTimestampMask = 3;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,120);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,110);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "qcrt";
    }

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() != 3) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end");
        }
        int64_t nStartTime, nTimeout;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        bool found = false;
        for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld\n", vDeploymentParams[0], nStartTime, nTimeout);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

/**
 * Regression network parameters overwrites for unit testing
 */
class CUnitTestParams : public CRegTestParams
{
public:
    explicit CUnitTestParams(const ArgsManager& args)
    : CRegTestParams(args)
    {
        // Activate the the BIPs for regtest as in Bitcoin
        consensus.QIP6Height = 0;
        consensus.QIP7Height = 0; // QIP7 activated on regtest

        consensus.nCoinbaseMaturity = 500;

        consensus.nCheckpointSpan = consensus.nCoinbaseMaturity*2; // Increase the check point span for the reorganization tests from 500 to 1000
    }
};

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams(gArgs));
    else if (chain == CBaseChainParams::UNITTEST)
        return std::unique_ptr<CChainParams>(new CUnitTestParams(gArgs));
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

std::string CChainParams::EVMGenesisInfo() const
{
    std::string genesisInfo = dev::eth::genesisInfo(GetEVMNetwork());
    ReplaceInt(consensus.QIP7Height,         "QIP7_STARTING_BLOCK", genesisInfo);
    ReplaceInt(consensus.QIP6Height,         "QIP6_STARTING_BLOCK", genesisInfo);
    ReplaceInt(consensus.nMuirGlacierHeight, "MUIR_STARTING_BLOCK", genesisInfo);
    return genesisInfo;
}

std::string CChainParams::EVMGenesisInfo(int nHeight) const
{
    std::string genesisInfo = dev::eth::genesisInfo(GetEVMNetwork());
    ReplaceInt(nHeight, "QIP7_STARTING_BLOCK", genesisInfo);
    ReplaceInt(nHeight, "QIP6_STARTING_BLOCK", genesisInfo);
    ReplaceInt(nHeight, "MUIR_STARTING_BLOCK", genesisInfo);
    return genesisInfo;
}

dev::eth::Network CChainParams::GetEVMNetwork() const
{
    return dev::eth::Network::qtumMainNetwork;
}

void CChainParams::UpdateBtcEcrecoverBlockHeight(int nHeight)
{
    consensus.QIP6Height = nHeight;
}

void UpdateBtcEcrecoverBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateBtcEcrecoverBlockHeight(nHeight);
}

void CChainParams::UpdateConstantinopleBlockHeight(int nHeight)
{
    consensus.QIP7Height = nHeight;
}

void UpdateConstantinopleBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateConstantinopleBlockHeight(nHeight);
}

void CChainParams::UpdateDifficultyChangeBlockHeight(int nHeight)
{
    consensus.posLimit = uint256S("0000000000003fffffffffffffffffffffffffffffffffffffffffffffffffff");
    consensus.fPowAllowMinDifficultyBlocks = false;
    consensus.fPowNoRetargeting = true;
    consensus.fPoSNoRetargeting = false;
}

void UpdateDifficultyChangeBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateDifficultyChangeBlockHeight(nHeight);
}

void CChainParams::UpdateOfflineStakingBlockHeight(int nHeight)
{
    consensus.nOfflineStakeHeight = nHeight;
}

void UpdateOfflineStakingBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateOfflineStakingBlockHeight(nHeight);
}

void CChainParams::UpdateDelegationsAddress(const uint160& address)
{
    consensus.delegationsAddress = address;
}

void UpdateDelegationsAddress(const uint160& address)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateDelegationsAddress(address);
}

void CChainParams::UpdatePowAllowMinDifficultyBlocks(bool fValue)
{
    consensus.fPowAllowMinDifficultyBlocks = fValue;
}

void UpdatePowAllowMinDifficultyBlocks(bool fValuet)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdatePowAllowMinDifficultyBlocks(fValuet);
}

void CChainParams::UpdatePowNoRetargeting(bool fValue)
{
    consensus.fPowNoRetargeting = fValue;
}

void UpdatePowNoRetargeting(bool fValuet)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdatePowNoRetargeting(fValuet);
}

void CChainParams::UpdatePoSNoRetargeting(bool fValue)
{
    consensus.fPoSNoRetargeting = fValue;
}

void UpdatePoSNoRetargeting(bool fValuet)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdatePoSNoRetargeting(fValuet);
}

void CChainParams::UpdateMuirGlacierHeight(int nHeight)
{
    consensus.nMuirGlacierHeight = nHeight;
}

void UpdateMuirGlacierHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateMuirGlacierHeight(nHeight);
}
