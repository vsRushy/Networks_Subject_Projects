#include "Networks.h"


#if defined(USE_TASK_MANAGER)

void ModuleResources::TaskLoadTexture::execute()
{
	*texture = App->modTextures->loadTexture(filename);
}

#endif


bool ModuleResources::init()
{
	background = App->modTextures->loadTexture("background.jpg");

#if !defined(USE_TASK_MANAGER)
	space = App->modTextures->loadTexture("space_background.jpg");
	space2 = App->modTextures->loadTexture("space_background_02.jpg");
	space3 = App->modTextures->loadTexture("space_background_03.jpg");
	space4 = App->modTextures->loadTexture("space_background_04.jpg");
	space5 = App->modTextures->loadTexture("space_background_05.jpg");
	space6 = App->modTextures->loadTexture("space_background_06.jpg");
	space7 = App->modTextures->loadTexture("space_background_07.jpg");
	asteroid1 = App->modTextures->loadTexture("asteroid1.png");
	asteroid2 = App->modTextures->loadTexture("asteroid2.png");
	spacecraft1 = App->modTextures->loadTexture("spacecraft1.png");
	spacecraft2 = App->modTextures->loadTexture("spacecraft2.png");
	spacecraft3 = App->modTextures->loadTexture("spacecraft3.png");
	loadingFinished = true;
	completionRatio = 1.0f;
#else
	loadTextureAsync("space_background.jpg", &space);
	loadTextureAsync("space_background_02.jpg", &space2);
	loadTextureAsync("space_background_03.jpg", &space3);
	loadTextureAsync("space_background_04.jpg", &space4);
	loadTextureAsync("space_background_05.jpg", &space5);
	loadTextureAsync("space_background_06.jpg", &space6);
	loadTextureAsync("space_background_07.jpg", &space7);
	loadTextureAsync("asteroid1.png",        &asteroid1);
	loadTextureAsync("asteroid2.png",        &asteroid2);
	loadTextureAsync("spacecraft1.png",      &spacecraft1);
	loadTextureAsync("spacecraft2.png",      &spacecraft2);
	loadTextureAsync("spacecraft3.png",      &spacecraft3);
	loadTextureAsync("laser.png",            &laser);
	loadTextureAsync("explosion1.png",       &explosion1);
#endif

	audioClipLaser = App->modSound->loadAudioClip("laser.wav");
	audioClipExplosion = App->modSound->loadAudioClip("explosion.wav");
	//App->modSound->playAudioClip(audioClipExplosion);

	return true;
}

Texture* ModuleResources::GetTexture(const std::string& name) const
{
	if (name.compare("background.jpg") == 0) return background;
	else if (name.compare("space_background.jpg") == 0) return space;
	else if (name.compare("space_background_02.jpg") == 0) return space2;
	else if (name.compare("space_background_03.jpg") == 0) return space3;
	else if (name.compare("space_background_04.jpg") == 0) return space4;
	else if (name.compare("space_background_05.jpg") == 0) return space5;
	else if (name.compare("space_background_06.jpg") == 0) return space6;
	else if (name.compare("space_background_07.jpg") == 0) return space7;
	else if (name.compare("asteroid1.png") == 0) return asteroid1;
	else if (name.compare("asteroid2.png") == 0) return asteroid2;
	else if (name.compare("spacecraft1.png") == 0) return spacecraft1;
	else if (name.compare("spacecraft2.png") == 0) return spacecraft2;
	else if (name.compare("spacecraft3.png") == 0) return spacecraft3;
	else if (name.compare("laser.png") == 0) return laser;
	else if (name.compare("explosion1.png") == 0) return explosion1;

	return nullptr;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::loadTextureAsync(const char * filename, Texture **texturePtrAddress)
{
	ASSERT(taskCount < MAX_RESOURCES);
	
	TaskLoadTexture *task = &tasks[taskCount++];
	task->owner = this;
	task->filename = filename;
	task->texture = texturePtrAddress;

	App->modTaskManager->scheduleTask(task, this);
}

void ModuleResources::onTaskFinished(Task * task)
{
	ASSERT(task != nullptr);

	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);

	for (uint32 i = 0; i < taskCount; ++i)
	{
		if (task == &tasks[i])
		{
			finishedTaskCount++;
			task = nullptr;
			break;
		}
	}

	ASSERT(task == nullptr);

	if (finishedTaskCount == taskCount)
	{
		finishedLoading = true;

		// Create the explosion animation clip
		explosionClip = App->modRender->addAnimationClip();
		explosionClip->frameTime = 0.1f;
		explosionClip->loop = false;
		for (int i = 0; i < 16; ++i)
		{
			float x = (i % 4) / 4.0f;
			float y = (i / 4) / 4.0f;
			float w = 1.0f / 4.0f;
			float h = 1.0f / 4.0f;
			explosionClip->addFrameRect(vec4{ x, y, w, h });
		}
	}
}

#endif
