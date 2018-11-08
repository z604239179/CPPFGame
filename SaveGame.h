#pragma once
class Controller;
static class SaveGame
{
public:
	static void Save(Controller* Ctrl);
	static void Read(Controller*& Ctrl);
};
