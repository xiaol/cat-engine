#include "sceneManager.h"
#include <string>
using namespace std;

sceneManager::sceneManager()
{
}

bool sceneManager::addNode(string strName, string strType )
{
//     void* pNode = new  hgeSprite;
//     m_Node.insert( make_pair( strName, pNode ) );
    return false;
}

sceneManager::~sceneManager()
{
	for ( map<string, void* >::iterator map_it = m_Node.begin(); map_it != m_Node.end(); map_it++ )
          delete map_it->second;
}