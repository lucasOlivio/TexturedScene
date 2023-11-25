#include "EngineMedia/MediaPlayer.h"
#include "components/Audio.h"
#include "components/Channel.h"
#include "components/AudioOcclusion.h"
#include "components/collision.h"
#include "components/Transform.h"
#include "common/utilsMat.h"

MediaPlayer* MediaPlayer::m_pInstance = nullptr;

MediaPlayer::MediaPlayer()
{
    this->m_pAudioManager = new AudioManager();

    // Singleton
    assert(!this->m_pInstance);
}

MediaPlayer::~MediaPlayer()
{
    delete this->m_pAudioManager;
}

MediaPlayer* MediaPlayer::Get()
{
    if (MediaPlayer::m_pInstance == nullptr)
    {
        MediaPlayer::m_pInstance = new MediaPlayer();
    }
    return MediaPlayer::m_pInstance;
}

bool MediaPlayer::Initialize(std::string baseAudiosPath, SceneView* pSceneView)
{
    this->m_pSceneView = pSceneView;

    this->m_pAudioManager->SetBasePath(baseAudiosPath);

    bool isAudioInit = this->m_pAudioManager->Initialize(this->m_pSceneView->GetNumComponents("channel"));
    if (!isAudioInit)
    {
        return false;
    }

    return true;
}

bool MediaPlayer::LoadScene()
{
    printf("Loading media player...\n");

    bool isChannelsLoaded = this->LoadChannels();
    if (!isChannelsLoaded)
    {
        return false;
    }

    bool isOcclusionsLoaded = this->LoadOcclusions();
    if (!isOcclusionsLoaded)
    {
        return false;
    }

    bool isAudiosLoaded = this->LoadAudios();
    if (!isAudiosLoaded)
    {
        return false;
    }

    return true;
}

bool MediaPlayer::LoadOcclusions()
{
    printf("Loading occlusions...\n");

    using namespace myutils;
    using namespace std;
    using namespace glm;

    this->m_pAudioManager->ClearOcclusions();

    for (this->m_pSceneView->First("audioOcclusion"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
    {
        EntityID entityId = this->m_pSceneView->CurrentKey();
        AudioOcclusionComponent* pAudioOcclusion = this->m_pSceneView->CurrentValue<AudioOcclusionComponent>();
        TransformComponent* pTransform = this->m_pSceneView->GetComponent<TransformComponent>(entityId,"transform");
        CollisionComponent* pCollision = this->m_pSceneView->GetComponent<CollisionComponent>(entityId, "collision");

        // For now only AABB for occlusion
        assert(pCollision->Get_eShape() == eShape::AABB);

        sAABB* pAABB = pCollision->GetShape<sAABB>();
        vector<vec3> vertices = GenerateRectangleVertices(pAABB->minXYZ, pAABB->maxXYZ);

        bool occlusionLoaded = this->m_pAudioManager->AddPolygon(pAudioOcclusion->GetDirect(), pAudioOcclusion->GetReverb(),
                                                             true, vertices, pTransform->GetPosition());

        if (!occlusionLoaded)
        {
            return false;
        }
    }

    return true;
}

bool MediaPlayer::LoadChannels()
{
    printf("Loading channels...\n");

    this->m_pAudioManager->ClearChannels();

    for (this->m_pSceneView->First("channel"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
    {
        ChannelComponent* pChannel = this->m_pSceneView->CurrentValue<ChannelComponent>();

        bool channelLoaded = this->m_pAudioManager->LoadChannel(pChannel->GetChannel());

        if (!channelLoaded)
        {
            return false;
        }
    }

    return true;
}

bool MediaPlayer::LoadAudios()
{
    printf("Loading audios...\n");

    this->m_pAudioManager->ClearAudios();

    for (this->m_pSceneView->First("audio"); !this->m_pSceneView->IsDone(); this->m_pSceneView->Next())
    {
        AudioComponent* pAudio = this->m_pSceneView->CurrentValue<AudioComponent>();
        FMOD::Sound* pSound = nullptr;

        bool audioLoaded = this->m_pAudioManager->LoadAudio(pAudio->GetFileName().c_str(), 
                                                            pAudio->IsStream(),
                                                            pAudio->IsLoop(),
                                                            pAudio->GetChannelGroup(),
                                                            &pSound);

        if (!audioLoaded)
        {
            return false;
        }

        pAudio->SetSound(pSound);
    }

    return true;
}

void MediaPlayer::Destroy()
{
    this->m_pAudioManager->Destroy();
    delete this->m_pAudioManager;
}

void MediaPlayer::Update(double deltaTime)
{
    this->m_pAudioManager->Update();
}

void MediaPlayer::SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity, 
                                        const glm::vec3& forward, const glm::vec3& up)
{
    this->m_pAudioManager->SetListenerAttributes(position, velocity, forward, up);
}

void MediaPlayer::SetAudio3DAttributes(EntityID entityId, const glm::vec3& position, const glm::vec3& velocity)
{
    int channelId = this->GetChannelId(entityId);

    this->m_pAudioManager->SetChannel3DAttributes(channelId, position, velocity);
}

void MediaPlayer::PlayAudio(EntityID entityId, glm::vec3 soundVelocity)
{
    AudioComponent* pAudio = this->m_pSceneView->GetComponent<AudioComponent>(entityId, "audio");
    TransformComponent* pTransform = this->m_pSceneView->GetComponent<TransformComponent>(entityId, "transform");

    int channelId = this->m_pAudioManager->PlayAudio(pAudio->GetFileName().c_str(),
                                     pAudio->GetChannelGroup(),
                                     pTransform->GetPosition(),
                                     soundVelocity,
                                     pAudio->GetMinDistance(),
                                     pAudio->GetMaxDistance());

    pAudio->SetChannelId(channelId);

    // Set audio default parameters
    this->m_pAudioManager->SetChannelVolume(channelId, pAudio->GetInitialVolume());
}

void MediaPlayer::StopAudio(EntityID entityId)
{
    int channelId = this->GetChannelId(entityId);

    this->m_pAudioManager->StopChannel(channelId);
}

void MediaPlayer::PauseAudio(EntityID entityId, bool value)
{
    int channelId = this->GetChannelId(entityId);

    this->m_pAudioManager->SetPaused(channelId, value);
}

bool MediaPlayer::SetPitch(EntityID entityId, float value)
{
    int channelId = this->GetChannelId(entityId);

    return this->m_pAudioManager->SetChannelPitch(channelId, value);
}

int MediaPlayer::GetChannelId(EntityID entityId)
{
    AudioComponent* pAudio = this->m_pSceneView->GetComponent<AudioComponent>(entityId, "audio");
    int channelId = pAudio->GetChannelId();

    return channelId;
}

bool MediaPlayer::IsPlaying(EntityID entityId)
{
    int channelId = this->GetChannelId(entityId);

    return this->m_pAudioManager->IsChannelPlaying(channelId);
}

bool MediaPlayer::IsFinished(EntityID entityId)
{
    int channelId = this->GetChannelId(entityId);

    bool isPlaying = this->m_pAudioManager->IsChannelPlaying(channelId);
    bool isPaused = this->m_pAudioManager->IsChannelPaused(channelId);

    if (!isPlaying && !isPaused)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MediaPlayer::SetDopplerLevel(EntityID entityId, float level)
{
    int channelId = this->GetChannelId(entityId);
        
    this->m_pAudioManager->SetDopplerLevel(channelId, level);
}

void MediaPlayer::SetReverberation(EntityID entityId, float decay, float density, float diffusion)
{
    int channelId = this->GetChannelId(entityId);
        
    this->m_pAudioManager->AddReverbFilterOnChannel(channelId);
    this->m_pAudioManager->SetReverbValuesOnChannel(channelId, decay, density, diffusion);
}

void MediaPlayer::SetDistortion(EntityID entityId, float level)
{
    int channelId = this->GetChannelId(entityId);
     
    this->m_pAudioManager->AddDistortionFilterOnChannel(channelId);
    this->m_pAudioManager->SetDistortionLevelFilterValuesOnChannel(channelId, level);
}

void MediaPlayer::SetHighpass(EntityID entityId)
{
    int channelId = this->GetChannelId(entityId);

    this->m_pAudioManager->AddHighPassFilterOnChannel(channelId);
}

void MediaPlayer::SetLowpass(EntityID entityId)
{
    int channelId = this->GetChannelId(entityId);

    this->m_pAudioManager->AddLowPassFilterOnChannel(channelId);
}
