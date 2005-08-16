#ifndef __INCLUDED__USELESS_DSOUND_SOUND_CARD_H__
#define __INCLUDED__USELESS_DSOUND_SOUND_CARD_H__

#include "Useless/Sound/Device/SoundCard.h"
#include "Useless/Util/PreInitializer.h"

#include <mmsystem.h>
#include <dsound.h>
#pragma comment (lib,"dsound.lib")


namespace Useless {

//USES
    class Screen;

class DSSoundCard : public SoundCard
{
public:
    DSSoundCard( const Screen &screen, bool use3DProcessing = false );
    virtual ~DSSoundCard();

    virtual Channel Play( Sample &sample, bool loop=false, bool pause=false );
    virtual void Validate( Sample &sample );

    virtual SampleData* CreateData( Snd::Properties &prop) const;

private:
    HWND _hWnd;

/* DirectSound static object
----------------------------------------------------------------*/
public:
    static LPDIRECTSOUND8 GetDSound() { return _p_direct_sound; }
    static LPDIRECTSOUNDBUFFER GetBuffer() { return _p_buffer; }
    static LPDIRECTSOUND3DLISTENER GetListener() { return _p_listener; }
    
private:
    static LPDIRECTSOUND8  _p_direct_sound;
    static LPDIRECTSOUNDBUFFER _p_buffer;
    static LPDIRECTSOUND3DLISTENER _p_listener;
    static void Init();
    static void Destroy();
    friend class PreInitializer<DSSoundCard>;
    friend bool IsSoundAvailable();
};

static PreInitializer<DSSoundCard> _init_dsound_object;

};//namespace Useless
#endif//__INCLUDED__USELESS_DSOUND_SOUND_CARD_H__
