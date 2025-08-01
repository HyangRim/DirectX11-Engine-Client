#include "pch.h"
#include "SoundManager.h"
#include <filesystem>

namespace fs = std::filesystem;

SoundManager::~SoundManager()
{
	Free();
}

HRESULT SoundManager::Init()
{
	ZeroMemory(m_Channels, sizeof(m_Channels));
	FMOD::System_Create(&m_System);

	m_System->init(32, FMOD_INIT_NORMAL, nullptr);
	if (m_System == nullptr)
		return E_FAIL;

	LoadSoundFile();

	return S_OK;
}

void SoundManager::PlaySound(const wstring& _keyname, const int _eID, const float _volume)
{
	auto iter = m_mapSound.find(_keyname);

	if (iter == m_mapSound.end()) return;

	FMOD_BOOL play = FALSE;
	m_System->playSound(iter->second, 0, false, &m_Channels[_eID]);
	m_Channels[_eID]->setVolume(_volume);

	m_System->update();
}

void SoundManager::PlayBGM(const wstring& _keyname, const float _volume)
{
	auto iter = m_mapSound.find(_keyname);

	if (iter == m_mapSound.end()) return;

	FMOD_BOOL play = FALSE;
	m_System->playSound(iter->second, 0, false, &m_Channels[0]);

	m_Channels[0]->setMode(FMOD_LOOP_NORMAL);
	m_Channels[0]->setVolume(_volume);

	m_System->update();
}

void SoundManager::StopSound(const int _eID)
{
	m_Channels[_eID]->stop();
}

void SoundManager::StopAll()
{
	for (int idx = 0; idx < MAXCHANNEL; ++idx) {
		m_Channels[idx]->stop();
	}
}

void SoundManager::SetChannelVolume(const int _eID, float _volume)
{
	m_Channels[_eID]->setVolume(_volume);
	m_System->update();
}

int SoundManager::VolumeUp(const int _eID, float _volume)
{
	m_volume += _volume;

	if(m_volume > 1.0f){
		m_volume = 1.0f;
	}

	m_Channels[_eID]->setVolume(m_volume);
	return 0;
}

int SoundManager::VolumeDown(const int _eID, float _volume)
{
	m_volume -= _volume;

	if (m_volume < 0.0f) {
		m_volume = 0.0f;
	}

	m_Channels[_eID]->setVolume(m_volume);
	return 0;
}

int SoundManager::Pause(const int _eID)
{
	m_pause = !m_Channels[_eID]->getPaused(&m_pause);
	m_Channels[_eID]->setPaused(m_pause);
	return 0;
}

void SoundManager::LoadSoundFile()
{
	const wstring soundFolderPath = L"..\\Resources\\Sounds\\";

	for (const auto& iter : fs::recursive_directory_iterator(soundFolderPath)) {
		if (!iter.is_regular_file())
			continue;

		//전체 경로, FMOD용 경로. 
		const fs::path& fullPath = iter.path();
		std::string sFullPath = fullPath.string();

		//새로운 Sound할당. 
		FMOD::Sound* sound = nullptr;
		FMOD_RESULT RESULT = m_System->createSound(sFullPath.c_str(), FMOD_LOOP_OFF, 0, &sound);

		if (RESULT == FMOD_OK) {
			fs::path relativePath = fs::relative(fullPath, soundFolderPath);
			std::wstring wkey = relativePath.generic_wstring();

			m_mapSound[wkey] = sound;
		}
	}

	m_System->update();
}


void SoundManager::Free()
{

	for (auto& pair : m_mapSound) {
		pair.second->release();
	}
	m_mapSound.clear();
	m_System->release();
	m_System->close();
}
