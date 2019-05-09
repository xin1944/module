
#include <iostream>
#include <string>
#include "json/json.h"
#include "json/writer.h"

using namespace std;

int main()
{
	Json::Value jRoot(Json::arrayValue);
	Json::Value obj;
	obj["no"] = 1;
	obj["channel_name"] = "channel2";
	obj["rms"] = 230.34;
	obj["secondary"] = 220.56;
	obj["unit"] = "V";
	jRoot.append(obj);
	obj["no"] = 100;
	jRoot.append(obj);
	Json::FastWriter jWriter;
	string str = jWriter.write(jRoot);
	cout<<str;
	return 0;
}

