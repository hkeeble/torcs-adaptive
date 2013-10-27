#include <iostream>
#include <string>

using namespace std;

struct watchItem
{
private:
	string tag;
	void* data;
public:
	watchItem(string tag, void* data);
	~watchItem();

	watchItem(const watchItem& param);
	watchItem& operator=(const watchItem& param);

	string GetTag() const;
	void* GetData() const;
};

class winTelemetry
{
public:
	winTelemetry();
	~winTelemetry();
	winTelemetry(const winTelemetry& param);
	void operator=(const winTelemetry& param);

private:
};