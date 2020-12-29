#pragma once


enum class BehaviourType : uint8;

struct Behaviour
{
	GameObject *gameObject = nullptr;
	GameObject* emitter = nullptr; // for the laser, to know the player that launched it

	bool isServer = false;
	bool isLocalPlayer = false;

	virtual BehaviourType type() const = 0;

	virtual void start() { }

	virtual void onInput(const InputController &input) { }

	virtual void update() { }

	virtual void destroy() { }

	virtual void onCollisionTriggered(Collider &c1, Collider &c2) { }

	virtual void write(OutputMemoryStream &packet) { }

	virtual void read(const InputMemoryStream &packet) { }
};


enum class BehaviourType : uint8
{
	None,
	Spaceship,
	Laser,
	Asteroid,
};


struct Laser : public Behaviour
{
	float secondsSinceCreation = 0.0f;

	BehaviourType type() const override { return BehaviourType::Laser; }

	void start() override;

	void update() override;
};


struct Spaceship : public Behaviour
{
	enum WEAPON
	{
		FRONT = 1,
		AXIS,
		DIAGONALS,
	};

	static const uint8 MAX_HIT_POINTS = 5;
	uint8 hitPoints = MAX_HIT_POINTS;

	int weapon = WEAPON::FRONT;
	int num_weapons = 3;

	GameObject *lifebar = nullptr;

	uint8 myScore = 0;

	void ScoreDeath(GameObject* killer);

	BehaviourType type() const override { return BehaviourType::Spaceship; }

	void start() override;

	void onInput(const InputController &input) override;

	void update() override;

	void destroy() override;

	void onCollisionTriggered(Collider &c1, Collider &c2) override;

	void write(OutputMemoryStream &packet) override;

	void read(const InputMemoryStream &packet) override;
};

struct Asteroid : public Behaviour
{
	static const uint8 MIN_HIT_POINTS = 1;
	static const uint8 MAX_HIT_POINTS = 5;  
	uint8 hitPoints = MIN_HIT_POINTS;

	vec2 speed = { 0,0 };

	std::pair<int, int> sizeRange = std::make_pair(50, 300);
	std::pair<int, int> hitPointRange = std::make_pair(1, 5);
	std::pair<int, int> speedRange = std::make_pair(5, 80);

	float secondsSinceCreation = 0.0f;

	float destroySeconds = 20.0f;

	BehaviourType type() const override { return BehaviourType::Asteroid; }

	void start() override;

	void update() override;

	void destroy() override;

	void onCollisionTriggered(Collider& c1, Collider& c2) override;

	void write(OutputMemoryStream& packet) override;

	void read(const InputMemoryStream& packet) override;
};
