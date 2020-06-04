#pragma once

#include <string>

class CSGO {
public:

    static const unsigned long handleSize = 0x4;
    static const unsigned long entityRefSize = 0x10;
    static const unsigned long MAX_PLAYERS = 64;
    static const std::string processName;
    static const std::string clientDllName;
    static const std::string engineDllName;

    static const int BoneOrder[];
    static const int BoneOrderSize = 19;
    static const int Bone_ProfileEnd = 2;
    static const int Bone_LeftArmEnd = 6;
    static const int Bone_RightArmEnd = 10;
    static const int Bone_TorsoEnd = 12;
    static const int Bone_LeftLegEnd = 15;
    static const int Bone_RightLegEnd = 18;

    static const unsigned long cs_gamerules_data = 0x0;
    static const unsigned long m_ArmorValue = 0xB368;
    static const unsigned long m_Collision = 0x320;
    static const unsigned long m_CollisionGroup = 0x474;
    static const unsigned long m_Local = 0x2FBC;
    static const unsigned long m_MoveType = 0x25C;
    static const unsigned long m_OriginalOwnerXuidHigh = 0x31C4;
    static const unsigned long m_OriginalOwnerXuidLow = 0x31C0;
    static const unsigned long m_SurvivalGameRuleDecisionTypes = 0x1320;
    static const unsigned long m_SurvivalRules = 0xCF8;
    static const unsigned long m_aimPunchAngle = 0x302C;
    static const unsigned long m_aimPunchAngleVel = 0x3038;
    static const unsigned long m_angEyeAnglesX = 0xB36C;
    static const unsigned long m_angEyeAnglesY = 0xB370;
    static const unsigned long m_bBombPlanted = 0x99D;
    static const unsigned long m_bFreezePeriod = 0x20;
    static const unsigned long m_bGunGameImmunity = 0x3930;
    static const unsigned long m_bHasDefuser = 0xB378;
    static const unsigned long m_bHasHelmet = 0xB35C;
    static const unsigned long m_bInReload = 0x32A5;
    static const unsigned long m_bIsDefusing = 0x391C;
    static const unsigned long m_bIsQueuedMatchmaking = 0x74;
    static const unsigned long m_bIsScoped = 0x3914;
    static const unsigned long m_bIsValveDS = 0x75;
    static const unsigned long m_bSpotted = 0x93D;
    static const unsigned long m_bSpottedByMask = 0x980;
    static const unsigned long m_bStartedArming = 0x33F0;
    static const unsigned long m_clrRender = 0x70;
    static const unsigned long m_dwBoneMatrix = 0x26A8;
    static const unsigned long m_fAccuracyPenalty = 0x3330;
    static const unsigned long m_fFlags = 0x104;
    static const unsigned long m_flC4Blow = 0x2990;
    static const unsigned long m_flDefuseCountDown = 0x29AC;
    static const unsigned long m_flDefuseLength = 0x29A8;
    static const unsigned long m_flFallbackWear = 0x31D0;
    static const unsigned long m_flFlashDuration = 0xA410;
    static const unsigned long m_flFlashMaxAlpha = 0xA40C;
    static const unsigned long m_flLastBoneSetupTime = 0x2924;
    static const unsigned long m_flLowerBodyYawTarget = 0x3A7C;
    static const unsigned long m_flNextAttack = 0x2D70;
    static const unsigned long m_flNextPrimaryAttack = 0x3238;
    static const unsigned long m_flSimulationTime = 0x268;
    static const unsigned long m_flTimerLength = 0x2994;
    static const unsigned long m_hActiveWeapon = 0x2EF8;
    static const unsigned long m_hMyWeapons = 0x2DF8;
    static const unsigned long m_hObserverTarget = 0x338C;
    static const unsigned long m_hOwner = 0x29CC;
    static const unsigned long m_hOwnerEntity = 0x14C;
    static const unsigned long m_iAccountID = 0x2FC8;
    static const unsigned long m_iClip1 = 0x3264;
    static const unsigned long m_iCompetitiveRanking = 0x1A84;
    static const unsigned long m_iCompetitiveWins = 0x1B88;
    static const unsigned long m_iCrosshairId = 0xB3D4;
    static const unsigned long m_iEntityQuality = 0x2FAC;
    static const unsigned long m_iFOV = 0x31E4;
    static const unsigned long m_iFOVStart = 0x31E8;
    static const unsigned long m_iGlowIndex = 0xA428;
    static const unsigned long m_iHealth = 0x100;
    static const unsigned long m_iItemDefinitionIndex = 0x2FAA;
    static const unsigned long m_iItemIDHigh = 0x2FC0;
    static const unsigned long m_iMostRecentModelBoneCounter = 0x2690;
    static const unsigned long m_iObserverMode = 0x3378;
    static const unsigned long m_iShotsFired = 0xA380;
    static const unsigned long m_iState = 0x3258;
    static const unsigned long m_iTeamNum = 0xF4;
    static const unsigned long m_lifeState = 0x25F;
    static const unsigned long m_nFallbackPaintKit = 0x31C8;
    static const unsigned long m_nFallbackSeed = 0x31CC;
    static const unsigned long m_nFallbackStatTrak = 0x31D4;
    static const unsigned long m_nForceBone = 0x268C;
    static const unsigned long m_nTickBase = 0x3430;
    static const unsigned long m_rgflCoordinateFrame = 0x444;
    static const unsigned long m_szCustomName = 0x303C;
    static const unsigned long m_szLastPlaceName = 0x35B4;
    static const unsigned long m_thirdPersonViewAngles = 0x31D8;
    static const unsigned long m_vecOrigin = 0x138;
    static const unsigned long m_vecVelocity = 0x114;
    static const unsigned long m_vecViewOffset = 0x108;
    static const unsigned long m_viewPunchAngle = 0x3020;
    static const unsigned long clientstate_choked_commands = 0x4D28;
    static const unsigned long clientstate_delta_ticks = 0x174;
    static const unsigned long clientstate_last_outgoing_command = 0x4D24;
    static const unsigned long clientstate_net_channel = 0x9C;
    static const unsigned long convar_name_hash_table = 0x2F0F8;
    static const unsigned long dwClientState = 0x589DD4;
    static const unsigned long dwClientState_GetLocalPlayer = 0x180;
    static const unsigned long dwClientState_IsHLTV = 0x4D40;
    static const unsigned long dwClientState_Map = 0x28C;
    static const unsigned long dwClientState_MapDirectory = 0x188;
    static const unsigned long dwClientState_MaxPlayer = 0x388;
    static const unsigned long dwClientState_PlayerInfo = 0x52B8;
    static const unsigned long dwClientState_State = 0x108;
    static const unsigned long dwClientState_ViewAngles = 0x4D88;
    static const unsigned long dwEntityList = 0x4D4B104;
    static const unsigned long dwForceAttack = 0x317C6EC;
    static const unsigned long dwForceAttack2 = 0x317C6F8;
    static const unsigned long dwForceBackward = 0x317C6A4;
    static const unsigned long dwForceForward = 0x317C710;
    static const unsigned long dwForceJump = 0x51F4D88;
    static const unsigned long dwForceLeft = 0x317C698;
    static const unsigned long dwForceRight = 0x317C6BC;
    static const unsigned long dwGameDir = 0x628678;
    static const unsigned long dwGameRulesProxy = 0x526807C;
    static const unsigned long dwGetAllClasses = 0xD5CF8C;
    static const unsigned long dwGlobalVars = 0x589AD8;
    static const unsigned long dwGlowObjectManager = 0x5292F20;
    static const unsigned long dwInput = 0x519C698;
    static const unsigned long dwInterfaceLinkList = 0x900584;
    static const unsigned long dwLocalPlayer = 0xD36B94;
    static const unsigned long dwMouseEnable = 0xD3C738;
    static const unsigned long dwMouseEnablePtr = 0xD3C708;
    static const unsigned long dwPlayerResource = 0x317AA3C;
    static const unsigned long dwRadarBase = 0x517FE5C;
    static const unsigned long dwSensitivity = 0xD3C5D4;
    static const unsigned long dwSensitivityPtr = 0xD3C5A8;
    static const unsigned long dwSetClanTag = 0x89F00;
    static const unsigned long dwViewMatrix = 0x4D3CA34;
    static const unsigned long dwWeaponTable = 0x519D15C;
    static const unsigned long dwWeaponTableIndex = 0x325C;
    static const unsigned long dwYawPtr = 0xD3C398;
    static const unsigned long dwZoomSensitivityRatioPtr = 0xD415E0;
    static const unsigned long dwbSendPackets = 0xD3ECA;
    static const unsigned long dwppDirect3DDevice9 = 0xA7030;
    static const unsigned long force_update_spectator_glow = 0x39CD52;
    static const unsigned long interface_engine_cvar = 0x3E9EC;
    static const unsigned long is_c4_owner = 0x3A9550;
    static const unsigned long m_bDormant = 0xED;
    static const unsigned long m_pStudioHdr = 0x294C;
    static const unsigned long m_pitchClassPtr = 0x5180100;
    static const unsigned long m_yawClassPtr = 0xD3C398;
    static const unsigned long model_ambient_min = 0x58CE4C;
    static const unsigned long set_abs_angles = 0x1D2CD0;
    static const unsigned long set_abs_origin = 0x1D2B10;

};