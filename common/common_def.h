#ifndef COMMON_DEF_H
#define COMMON_DEF_H

enum MessageId
{
    kRegisterReq = 1000,
    kRegisterAck,
    kLoginReq,
    kLoginAck,
    kLandingReq,
    kLandingAck,
    kAuthReq,
    kAuthAck,
    kQuitReq,
    kQuitAck,
    kMax
};

#endif // COMMON_DEF_H
