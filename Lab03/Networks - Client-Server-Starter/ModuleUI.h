#pragma once

enum class UI_STYLE
{
	DARK,
	GREEN,
	DARK_BLUE
};

class ModuleUI : public Module
{
public:

	// Virtual functions

	bool init() override;

	bool preUpdate() override;

	bool gui() override;

	bool postUpdate() override;

	bool cleanUp() override;


	// Public methods

	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void SetUIStyle(UI_STYLE style);
};
