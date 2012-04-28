#include <hgeSprite.h>
#include <map>
#include <string>
using namespace std;

extern HGE* hge;


class sceneManager
{
public:
    sceneManager();
    bool updateScene();
    bool renderScene();
    bool addNode( string strName, string strType );
	~sceneManager();
private:
    map<string, void* > m_Node;
};
