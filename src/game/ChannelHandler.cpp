/*
 * BlizzLikeCore integrates as part of this file: CREDITS.md and LICENSE.md
 */

#include "ObjectMgr.h"                                      // for normalizePlayerName
#include "ChannelMgr.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(AllianceChannelMgr);
INSTANTIATE_SINGLETON_1(HordeChannelMgr);

void WorldSession::HandleChannelJoin(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    uint32 channel_id;
    uint8 unknown1, unknown2;
    std::string channelname, pass;

    recvPacket >> channel_id >> unknown1 >> unknown2;
    recvPacket >> channelname;

    if (channelname.empty())
        return;

    recvPacket >> pass;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetJoinChannel(channelname, channel_id))
            chn->Join(_player->GetGUID(), pass.c_str());
}

void WorldSession::HandleChannelLeave(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    uint32 unk;
    std::string channelname;
    recvPacket >> unk;                                      // channel id?
    recvPacket >> channelname;

    if (channelname.empty())
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Leave(_player->GetGUID(), true);
        cMgr->LeftChannel(channelname);
    }
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->List(_player);
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, pass;
    recvPacket >> channelname;
    recvPacket >> pass;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Password(_player->GetGUID(), pass.c_str());
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, newp;
    recvPacket >> channelname;
    recvPacket >> newp;

    if (!normalizePlayerName(newp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->SetOwner(_player->GetGUID(), newp.c_str());
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->SendWhoOwner(_player->GetGUID());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->SetModerator(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->UnsetModerator(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->SetMute(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->UnsetMute(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Invite(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Kick(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Ban(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname, otp;
    recvPacket >> channelname;
    recvPacket >> otp;

    if (!normalizePlayerName(otp))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->UnBan(_player->GetGUID(), otp.c_str());
}

void WorldSession::HandleChannelAnnounce(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Announce(_player->GetGUID());
}

void WorldSession::HandleChannelModerate(WorldPacket& recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->Moderate(_player->GetGUID());
}

void WorldSession::HandleChannelRosterQuery(WorldPacket &recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->List(_player);
}

void WorldSession::HandleChannelInfoQuery(WorldPacket &recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;
    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
        {
            WorldPacket data(SMSG_CHANNEL_MEMBER_COUNT, chn->GetName().size()+1+1+4);
            data << chn->GetName();
            data << uint8(chn->GetFlags());
            data << uint32(chn->GetNumPlayers());
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleChannelJoinNotify(WorldPacket &recvPacket)
{
    sLog.outDebug("Opcode %u", recvPacket.GetOpcode());

    std::string channelname;
    recvPacket >> channelname;
    /*if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->JoinNotify(_player->GetGUID());*/
}

