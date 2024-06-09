#include "Audio.h"
#include "AudioError.h"
#include <thread>

// Use to output utf-16 on windows console
#include <io.h>
#include <fcntl.h>
#include <iostream>

#include <Windows.h>
#include <conio.h>
#include <random>

#include <mutex>

using namespace AudioSystem;

auto tick(AudioEngine& engine, Audio& audio) -> void;
auto getMusicList(const wchar_t* audioPath) -> std::vector<std::wstring>;

static std::random_device rd;
static std::mt19937 gen(rd());

auto wmain(int argc, wchar_t** argv) -> int
{
    // Use to output utf-16 on windows console
    _setmode(_fileno(stdout), _O_U16TEXT);

    if (argc != 2 || !std::filesystem::is_directory(argv[1]))
    {
        std::cout << "[usage] Audio.exe <Directory of audios>\n";
        return 1;
    }

    AudioEngine engine;
    Audio       audio(engine);

    bool end{ false };

    auto musicList{ getMusicList(argv[1]) };

    if (musicList.empty())
    {
        std::cout << "Empty directory.\n";
        return 1;
    }

    int index{ 0 };
    audio.load(musicList[index]);

    std::mutex mutex;

    std::jthread loop{
        [&]
        {
            while (!end)
            {
                tick(engine, audio);



                std::this_thread::sleep_for(std::chrono::milliseconds(10));


                std::wcout << std::format
                (
                    L"Filename : {}\n"
                    L"Time     : {}\n"
                    L"Duration : {}\n"
                    L"Volume   : {}\n"
                    L"Paused   : {}\n"
                    L"Loop     : {}\n",
                    audio.getFileName(), audio.getTime(), audio.getDuration(), audio.getVolume(),
                    (audio.isPaused() ? L"True" : L"False"),
                    (audio.isLoop() ? L"True" : L"False")
                );

                if (audio.isEnd())
                {
                    index = std::clamp(++index, 0, static_cast<int>(musicList.size() - 1));
                    audio.load(musicList[index]);
                    audio.play();
                }
                system("cls");
            }
        }
    };

    char command{};
    while (command != 'q')
    {
        command = _getch();
      
         switch (command)
         {
         case ' ':
             if (audio.isPaused())
             {
                 audio.play();
             }
             else {
                 audio.pause();
             }
             break;

         case 'l':
             audio.setTime(audio.getTime() + 5);
             break;

         case 'h':
             audio.setTime(audio.getTime() - 5);
             break;

         case 'k':
             audio.setVolume(audio.getVolume() + 2);
             break;

         case 'j':
             audio.setVolume(audio.getVolume() - 2);
             break;

         case 'c':
             audio.setLoop(!audio.isLoop());
             break;

         case 'n':
         {
             index = std::clamp(++index, 0, static_cast<int>(musicList.size() - 1));
             auto pause{ audio.isPaused() };
             audio.load(musicList[index]);
             if (!pause)
             {
                 audio.play();
             }
         } break;

         case 'p':
         {
             index = std::clamp(--index, 0, static_cast<int>(musicList.size() - 1));
             auto pause{ audio.isPaused() };
             audio.load(musicList[index]);
             if (!pause)
             {
                 audio.play();
             }
         } break;

         case 'a':
         {
             std::shuffle(musicList.begin(), musicList.end(), gen);
             auto pause{ audio.isPaused() };
             index = 0;
             audio.load(musicList[index]);
             if (!pause)
             {
                 audio.play();
             }
         } break;

         default:
             break;
         }
        }

        end = true;
    }

    auto tick(AudioEngine & engine, Audio & audio) -> void
    {
        engine.update();
    }

    auto getMusicList(const wchar_t* audioPath) -> std::vector<std::wstring>
    {
        std::vector<std::wstring> musicList;

        for (const auto& entry : std::filesystem::directory_iterator{ audioPath })
        {
            if (isSupportedType(entry.path()))
            {
                musicList.emplace_back(entry.path().wstring());
            }
        }

        return musicList;
    }