# Music Player

A Universal Snowdrop Modloader plugin that provides folder-based custom radio stations.

## Content layout

Place a folder beside `MusicPlayer.dll` named `MusicPlayer`:

```
MusicPlayer.dll
MusicPlayer/
  stations/
    My Station/
      station.ini
      intro/
      music/
      interstitial/
      sfx/
```

Supported audio formats are MP3, WAV and FLAC.

## Station INI

All station settings and track metadata live in the station's single `station.ini`:

```ini
[Station]
Name=My Station
Description=An example station.

[Playback]
ShuffleMusic=true
ShuffleInterstitials=true
MinMusicBetweenInterstitials=3
MaxMusicBetweenInterstitials=5

[Audio]
MusicVolume=100
InterstitialVolume=100
SfxVolume=35

[Example Song.mp3]
Title={Song Title}
Artist={Artist Name}

[Static Loop.wav]
Role=Background
Loop=true
Volume=60
```

If a music file has no metadata section, its filename stem is used as the title and `Unknown Artist` is used as the artist.

SFX sections support `Role=Background`, `Role=Transition`, or `Role=OneShot`. Background sounds are looped when `Loop=true`; transition sounds are played when tuning to a station.
