// movie.h
//

#pragma once

struct Kit_Source;
struct Kit_Player;
struct Kit_PlayerInfo;
struct SDL_Texture;

#define AUDIOBUFFER_SIZE (32768)

class CMovie
{
public:
    CMovie();
    ~CMovie();

    bool    Create();
    bool    GetEnable();
    bool    IsExist (const char *pFilename);
    bool    Play (RECT rect, const char *pFilename);
    void    Stop();
    void    Pause();
    void    Resume();
    bool    Render();

protected:
    void    playMovie();
    bool    fileOpenMovie (RECT rect, const char *pFilename);
    void    fileCloseMovie();
    void    termAVI();
    bool    initAVI();

protected:
    Kit_PlayerInfo *pinfo;
    Kit_Source  *m_movie;
    Kit_Player  *m_player;
    SDL_Texture *m_videoTex;
    Sint32 m_ret;
    SDL_AudioDeviceID m_audioDev;

    char        m_audiobuf[AUDIOBUFFER_SIZE];
    bool        m_bEnable;
    bool        m_fPlaying;     // Play flag: true == playing, false == paused
    bool        m_fMovieOpen;   // Open flag: true == movie open, false = none
};

