#include "Networks.h"
#include "ReplicationManagerClient.h"
#include "ReplicationCommand.h"

// TODO(you): World state replication lab session
void ReplicationManagerClient::read(const InputMemoryStream& packet) const
{
	while (packet.RemainingByteCount() > 0)
	{
		ReplicationCommand replicationCommand;

		packet >> replicationCommand.networkId;
		packet >> replicationCommand.action;

		GameObject* gameObject = nullptr;

		switch (replicationCommand.action)
		{
		case ReplicationAction::Create:
		{
			gameObject = App->modGameObject->Instantiate();
			App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, replicationCommand.networkId);

			packet >> gameObject->playerName >> gameObject->position.x >> gameObject->position.y >>
				gameObject->size.x >> gameObject->size.y >>
				gameObject->angle >> gameObject->tag >>
				gameObject->networkInterpolationEnabled;

			// Setup interpolation
			gameObject->initial_position = gameObject->position;
			gameObject->initial_angle = gameObject->angle;

			gameObject->final_position = gameObject->position;
			gameObject->final_angle = gameObject->angle;

			bool has_sprite = false;
			packet >> has_sprite;
			if (has_sprite)
			{
				gameObject->sprite = App->modRender->addSprite(gameObject);
				std::string texture_name;
				packet >> texture_name;
				gameObject->sprite->texture = App->modResources->GetTexture(texture_name);
				packet >> gameObject->sprite->order;
			}

			bool has_animation = false;
			packet >> has_animation;
			if (has_animation)
			{
				Animation::AnimationType animation_type = Animation::AnimationType::None;
				packet >> animation_type;

				switch (animation_type)
				{
				case Animation::AnimationType::Explosion:
				{
					gameObject->animation = App->modRender->addAnimation(gameObject);
					// TODO: Change addAnimationMethod (refactor)
					if (gameObject->animation != nullptr)
					{
						gameObject->animation->clip = App->modResources->explosionClip;
					}
					
					break;
				}
				default:
				{
					break;
				}
				}
			}

			bool has_collider = false;
			packet >> has_collider;
			if (has_collider)
			{
				ColliderType collider = ColliderType::None;
				packet >> collider;
				gameObject->collider = App->modCollision->addCollider(collider, gameObject);
				packet >> gameObject->collider->isTrigger;
			}

			bool has_behaviour = false;
			packet >> has_behaviour;
			if (has_behaviour)
			{
				BehaviourType b_type = BehaviourType::None;
				packet >> b_type;

				gameObject->behaviour = App->modBehaviour->addBehaviour(b_type, gameObject);
				gameObject->behaviour->read(packet);
			}

			break;
		}
		case ReplicationAction::Update:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);

			if (gameObject != nullptr)
			{
				if (gameObject->networkInterpolationEnabled)
				{
					// Reset secondsElapsed
					gameObject->secondsElapsed = 0.0f;

					// Reset interpolation position
					gameObject->initial_position = gameObject->position;

					// Read final position from interpolation
					//packet >> gameObject->final_position.x >> gameObject->final_position.y;
					packet >> gameObject->position.x >> gameObject->position.y; // needs fix
				}
				else
				{
					packet >> gameObject->position.x >> gameObject->position.y;
				}

				packet >> gameObject->size.x >> gameObject->size.y;
				
				if (gameObject->networkInterpolationEnabled)
				{
					// Reset interpolation angle
					gameObject->initial_angle = gameObject->angle;

					// Read final angle from interpolation
					//packet >> gameObject->final_angle;
					packet >> gameObject->angle; // needs fix
				}
				else
				{
					packet >> gameObject->angle;
				}
				
				packet >> gameObject->tag;

				bool has_behaviour = false;
				packet >> has_behaviour;
				if (has_behaviour)
				{
					gameObject->behaviour->read(packet);
				}
			}

			break;
		}
		case ReplicationAction::Destroy:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);
			if (gameObject != nullptr)
			{
				App->modLinkingContext->unregisterNetworkGameObject(gameObject);
				App->modGameObject->Destroy(gameObject);
			}

			break;
		}
		default:
		{
			break;
		}
		}
	}
}
