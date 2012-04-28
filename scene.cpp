
#include "scene.h"
#include <math.h>

extern HGE* hge;
extern float fPos[2];
extern bool g_bLeaderInit;
extern float fStep;

//-------------------------------------------------XEntity-------------------------------------------

XEntity::XEntity( const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, 
	int iFrame /* = 4 */, EENTITYS eType /*= HGE_SPRITE*/, int iMeshCol, int iMeshRow )
{
	switch( eType )
	{
	case HGE_SPRITE:
		{
			m_tex =  hge->Texture_Load( szFileName );
			m_pSr.pSr = new hgeSprite( m_tex, fStartX, fStartY, fWidth, fHeight*iFrame );
			m_pSr.pSr->SetBlendMode( BLEND_DEFAULT_Z );
			m_pSr.pSr->SetTextureRect( fStartX, fStartY, fWidth, fHeight );
		}
		break;
	case HGE_DISTORTIONMESH:
		{
			m_tex =  hge->Texture_Load( szFileName );
			m_pSr.pMesh = new hgeDistortionMesh( iMeshCol, iMeshRow );

			m_pSr.pMesh->SetTexture(m_tex);
			m_pSr.pMesh->SetTextureRect(fStartX,fStartY,fWidth,fHeight);
			m_pSr.pMesh->SetBlendMode( BLEND_DEFAULT_Z );

		}
		break;
	}

	
    m_fmass = 0.0f;
    m_pScene = NULL;
	m_pEtNext = NULL;
	m_eStatus = INSCENE;
	m_fPosX = 0.0f;
	m_fPosY = 0.0f;
	m_fRot = 0.0f;

	m_fStartXTex = fStartX;
	m_fStartYTex = fStartY;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_iFrame = iFrame;

}

XEntity::~XEntity()
{
	RELEASE( m_pSr.pSr, m_tex )
}

void XEntity::setScene(XScene* pScene)
{
	m_pScene = pScene;
}

void XEntity::setPos( float fX, float fY )
{
	m_fPosX = fX;
	m_fPosY = fY;
}

bool XEntity::frameImage()
{
	static int iCount = 0;
	m_pSr.pSr->SetTextureRect( m_fStartXTex, m_fStartYTex + iCount*m_fHeight, m_fWidth, m_fHeight );
	iCount++;
	iCount %= m_iFrame;
	return false;
}

bool XEntity::collisionChain( XEntity* pCollisionA )
{
	return false;
}

//---------------------------------------------------------------XScene-----------------------------------
XScene::XScene()
{
	m_pListEntity = NULL;
	m_fElapse = 0.0f;
	m_fTotalTime = 0.0f;
	for( int i =0; i < DIMENTION; i++ )
	{
		for ( int j = 0; j < MAX_OBJECTS; j++ )
		{
			m_ppEtCollision[i][j] = NULL;
		}    
	}

	m_iSceneNum = 0;
	m_iScenePos = 0;

	walkingZone.push_back( Point( 0,403) );
	walkingZone.push_back( Point( 1600,456) );
	walkingZone.push_back( Point( 1600,1200) );
	walkingZone.push_back( Point( 0,1200 ) );
	walkingZone.push_back( Point( 0,403) );
}

XScene::~XScene()
{
	XEntity* pEntityList = m_pListEntity;
	while( pEntityList != NULL )
	{
		XEntity* pTemp= pEntityList->m_pEtNext;
		delete pEntityList;
        pEntityList = NULL;
		pEntityList = pTemp;
	}
}

bool XScene::addEntity( EENTITYS eType, EETLEVEL eLev )
{
	XEntity* pEntity;
	switch ( eType )
	{
	case ETBUBBLE:
		pEntity = new XEtBubble( "media/bubble.png", 38.0f, 8.0f, 29.0f, 27.0f, 4 );
		break;
	case ETSCENE01:
		{
			XEtScene* pEtScene = new XEtScene( "media/scene01.png",0.0f,0.0f, WINDOW_W, WINDOW_H*2.0f );
			pEtScene->addMeshNode("media/scene01_1.png" ,0.0f, 0.0f, 603.0f, 141.0f, 196.0f, 171.0f );
			pEntity = pEtScene;
			addScenetoList( pEntity );
			setCurrentScene( 0 );
		}
		break;
	case ETLEADER01:
		{
			pEntity = new XEtLder( "media/leader01.png", 0.0f,0.0f, 56.0f,61.0f,1 );
			pEntity->setPos( 178.0f, 293.0f );
		}
		break;
	default:
		break;
	}	
	if ( m_pListEntity == NULL )
		m_pListEntity = pEntity;
	else
	{
		XEntity* pTemp = m_pListEntity;
		while ( pTemp)	
		{
			if ( pTemp->m_pEtNext == NULL )
			{
				pTemp->m_pEtNext = pEntity;
				break;
			}
			else 
				pTemp = pTemp->m_pEtNext;
		}
	}

	XEntity** pTempB = *(m_ppEtCollision + eLev) ;
	for ( int i = 0; i < MAX_OBJECTS; i++ )
	{
		if ( pTempB[i] != NULL )
			continue;
		else
		{
			pTempB[i] = pEntity;
			break;
		}
	}

	pEntity->setScene( this );
	return false;
}

bool XScene::render()
{
	XEntity* pTemp;
	pTemp = m_pListEntity;
	while ( pTemp )
	{
		pTemp->render();
		pTemp = pTemp->m_pEtNext;
	}
	return false;
}

bool XScene::update( int iBe )
{
	m_iBehaviors = iBe;
	XEntity* pTemp;
	pTemp = m_pListEntity;
	while ( pTemp )
	{
		pTemp->update( pTemp->onBehaviors( m_iBehaviors ) );
		pTemp = pTemp->m_pEtNext;
	}

	for ( int i = 0; i < MAX_OBJECTS; i++ )
	{
		if ( m_ppEtCollision[ONE][i] == NULL )
			 break;
		for ( int j = i + 1; j < MAX_OBJECTS; j++ )
		{
			if ( m_ppEtCollision[ONE][j] == NULL )
				break;
			if ( collisionDetect( m_ppEtCollision[ONE][i], m_ppEtCollision[ONE][j] ))
			{
				m_ppEtCollision[ONE][i]->update( COLLISION );
				m_ppEtCollision[ONE][j]->update( COLLISION );
			}
		}
	}

	return false;
}

bool XScene::collisionDetect( XEntity* pEntityA, XEntity* pEntityB )
{
	for ( GeometryTools::Polyline::iterator it_vec = pEntityA->m_vecPtList.begin() ;
		it_vec != pEntityA->m_vecPtList.end(); it_vec++)
	{
		if ( GeometryTools::GetPosOfPolygon( *it_vec, pEntityB->m_vecPtList ) == GeometryTools::PtOnBorder || 
			GeometryTools::GetPosOfPolygon( *it_vec, pEntityB->m_vecPtList ) == GeometryTools::PtInPolygon ) 
			return true;
	}
	return false;
}

void XScene::setPassTime( float fElapseT )
{
	m_fElapse = fElapseT;
    m_fTotalTime += fElapseT;
}

float XScene::getElapseTime()
{
	return m_fElapse;
}

float XScene::getTotalTime()
{
	return m_fTotalTime;
}

void XScene::addScenetoList( XEntity* pScene )
{
	m_pSceneList[ m_iSceneNum ] =pScene;
	m_iSceneNum++;
}

XEntity* XScene::getCurrentScene()
{
	return m_pSceneList[ m_iScenePos ];
}

EETSTATUS XScene::getCurrentSceneStatus()
{
	return m_pSceneList[ m_iScenePos ]->m_eStatus;
}

void XScene::setCurrentScene( int iPos )
{
	m_iScenePos = iPos;
}

bool XScene::isInWalkingZone()
{
	Point pt(fPos[0],fPos[1]);
	Point ptScene = windowToScene( pt );
	GeometryTools::PtToPolygon eSt = (GeometryTools::PtToPolygon)GeometryTools::GetPosOfPolygon( ptScene, walkingZone );
	if ( eSt == GeometryTools::PtOnBorder || 
		eSt == GeometryTools::PtInPolygon ) 
		return true;
	else
		return false;
}

bool XScene::isInWalkingZone( Point pt )
{
	Point ptScene = windowToScene( pt );
	if ( GeometryTools::GetPosOfPolygon( ptScene, walkingZone ) == GeometryTools::PtOnBorder || 
		GeometryTools::GetPosOfPolygon( ptScene, walkingZone ) == GeometryTools::PtInPolygon ) 
		return true;
	else
		return false;
}
Point XScene::windowToScene( const Point pt )
{
	Point ptScene;
	XEntity* pScene = m_pSceneList[m_iScenePos];
	ptScene.x = pt.x + pScene->m_fStartXTex;
	ptScene.y = pt.y + pScene->m_fStartYTex;
	return ptScene;
}
//----------------------------------------------------XEtBubble----------------------------------

XEtBubble::XEtBubble( const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, int iFrame /* = 4 */ )
:XEntity(szFileName, fStartX, fStartY, fWidth, fHeight, iFrame )
{
	hge->Random_Seed();
	m_fScale = hge->Random_Float( 0.8f, 1.6f);
	m_fInterval = hge->Random_Float( -3.0f, -10.0f);
	m_fVelocity = hge->Random_Float( 6.0f, 15.0f);
	m_fAmplitude = hge->Random_Float( 8.0f, 13.0f);
	
	
	m_fPosX = hge->Random_Float( 0, WINDOW_W );
	m_fPosY = hge->Random_Float( 0, WINDOW_H );

	do
	{
		m_vecPtList.push_back( Point( m_fPosX + fWidth, m_fPosY + 0.0 ) );
		m_vecPtList.push_back( Point( m_fPosX + fWidth, m_fPosY + fHeight/2 ));
		m_vecPtList.push_back( Point( m_fPosX + fWidth/2, m_fPosY + fHeight ));
		m_vecPtList.push_back( Point( m_fPosX + 0.0, m_fPosY + fHeight/2 ));
		m_vecPtList.push_back( Point( m_fPosX + fWidth, m_fPosY + 0.0 ));
	}while( false );

	m_fDy = 0.2f;
	m_fElapseT[0] = 0.0f;
	m_fElapseT[1] = 0.0f;
	m_fTime = m_fInterval/ABS(m_fVelocity);
}

XEtBubble::~XEtBubble()
{
}

bool XEtBubble::render()
{
	m_pSr.pSr->RenderEx( getPosX(), getPosY(), 0.0f ,m_fScale );
	return false;
}

bool XEtBubble::update(ENTITYBEHAVIOR eBe)
{
	float fX = getPosX();
	float fY = getPosY();
	switch ( eBe )
	{
	case FRAMEIMAGE:
		frameImage();
	case UNRESTRAINED:
		{
			//static float fDy = 0.2f;

			if ( fY < 0.0f )
				setPos( fX, WINDOW_H );
			else
				setPos ( fX + m_fAmplitude*sin( m_fDy ), getPosY() + m_fInterval );
			if ( !m_pScene->isInWalkingZone( Point( getPosX(), getPosY() )) )
				m_pSr.pSr->SetColor( SETA(  m_pSr.pSr->GetColor(), 0 ) );
			else
				m_pSr.pSr->SetColor( SETA( m_pSr.pSr->GetColor(), 255 ) );
			m_fDy += 0.2f;

			break;
		}
	case COLLISION:
		m_pSr.pSr->SetColor( SETA(  m_pSr.pSr->GetColor(), 0 )  );
		break;
	default:
		break;
	}

	for ( GeometryTools::Polyline::iterator it_vec = m_vecPtList.begin(); it_vec != m_vecPtList.end();
		it_vec++ )
	{
		(*it_vec).x += (getPosX() - fX);
		(*it_vec).y += (getPosY() - fY);
	}
	return false;
}

ENTITYBEHAVIOR XEtBubble::onBehaviors( int eBes )
{
    for( int i =0; i < 2; i++ )
	{
		m_fElapseT[i] += m_pScene->getElapseTime();
	}
	if ( m_fElapseT[0] > m_fTime )
	{
		m_fElapseT[0] = 0.0f;
		return UNRESTRAINED;
	}
	else if ( m_fElapseT[1] > 1.5f )
	{
		m_fElapseT[1] = 0.0f;
		return FRAMEIMAGE;
	}
	else 
		return IDLE;
}

bool XEtBubble::frameImage()
{
	//m_fScale = hge->Random_Float( 1.0f,2.0f );
	XEntity::frameImage();
	return false;
}


//--------------------------------------------XEtScene------------------------------------

XEtScene::XEtScene(const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, int iFrame /* = 1 */):
XEntity(szFileName,fStartX,fStartY,fWidth,fHeight,iFrame)
{
	m_fStartXTex = 0;
	m_fStartYTex = WINDOW_H;
	m_fWidth = WINDOW_W;
	m_fHeight = WINDOW_H;

	m_pSr.pSr->SetTextureRect( 0, WINDOW_H, WINDOW_W, WINDOW_H );
	m_iMesh = 0;
}

XEtScene::~XEtScene( )
{
	for ( int i = 0; i < m_iMesh; i++ )
	{
		delete m_meshArray[m_iMesh];
		m_meshArray[m_iMesh] = NULL;
		hge->Texture_Free( m_texArray[m_iMesh] );
	}
}

bool XEtScene::update(ENTITYBEHAVIOR eBe)
{
	float fT = m_pScene->getTotalTime();
	switch( eBe )
	{
	case SCENE_SCROLL_UP:
		{
			m_fStartYTex -= SCROLL_INTERVAL_Y;
			m_eStatus = MOVING;
			if ( m_fStartYTex < 0)
			{ m_fStartYTex = 0.0f; m_eStatus = CEASE;}
			m_pSr.pSr->SetTextureRect( m_fStartXTex, m_fStartYTex, WINDOW_W, WINDOW_H );
		}
		break;
	case SCENE_SCROLL_DOWN:
		{
			m_fStartYTex += SCROLL_INTERVAL_Y;
			m_eStatus = MOVING;
			if ( m_fStartYTex > WINDOW_H )
			{m_fStartYTex = WINDOW_H; m_eStatus = CEASE;}
			m_pSr.pSr->SetTextureRect( m_fStartXTex, m_fStartYTex, WINDOW_W, WINDOW_H );
		}
		break;
	case SCENE_SCROLL_LEFT:
		{
			m_fStartXTex -= SCROLL_INTERVAL_X;
			m_eStatus = MOVING;
			if ( m_fStartXTex < 0 )
			{m_fStartXTex = 0; m_eStatus = CEASE;}
			m_pSr.pSr->SetTextureRect( m_fStartXTex, m_fStartYTex, WINDOW_W, WINDOW_H );
		}
		break;
	case SCENE_SCROLL_RIGHT:
		{
			m_fStartXTex += SCROLL_INTERVAL_X;
			m_eStatus = MOVING;
			if ( m_fStartXTex > WINDOW_W )
			{m_fStartXTex = WINDOW_W; m_eStatus = CEASE;}
			m_pSr.pSr->SetTextureRect( m_fStartXTex, m_fStartYTex, WINDOW_W, WINDOW_H );
		}
		break;
	default:
		{
			for ( int iNum = 0; iNum < m_iMesh; iNum++ )
			{
				for( int i=1; i<MESHROW-1; i++)
					for( int j=1; j<MESHCOL-1; j++)
					{
						m_meshArray[iNum]->SetDisplacement(j,i,cosf(fT*3+(i+j)/2)*5,sinf(fT*3+(i+j)/2)*5,
							HGEDISP_NODE);
					}
			}
		}
		break;
	}

	return false;
}

ENTITYBEHAVIOR XEtScene::onBehaviors( int iBes )
{
	if ( iBes & SCENE_SCROLL_UP )
		return SCENE_SCROLL_UP;
	else if ( iBes & SCENE_SCROLL_DOWN )
		return SCENE_SCROLL_DOWN;
	else if ( iBes & SCENE_SCROLL_LEFT )
		return SCENE_SCROLL_LEFT;
	else if ( iBes & SCENE_SCROLL_RIGHT )
		return SCENE_SCROLL_RIGHT;
	return IDLE;
}

bool XEtScene::frameImage()
{
	return false;
}

bool XEtScene::render()
{
	m_pSr.pSr->Render( getPosX(), getPosY() );
	static float s_fLastSceneY = m_fStartYTex;
	static float s_fLastSceneX = m_fStartXTex;

	for ( int i = 0; i < m_iMesh; i++ )
	{
		m_meshPosArray[i].y +=  s_fLastSceneY - m_fStartYTex;
		m_meshPosArray[i].x += s_fLastSceneX - m_fStartXTex;

		m_meshArray[i]->Render( m_meshPosArray[i].x, m_meshPosArray[i].y  );
	}
	s_fLastSceneY = m_fStartYTex;
	s_fLastSceneX = m_fStartXTex;
	return false;
}

bool XEtScene::addMeshNode( const char* szTexPath, float fMeshX, float fMeshY, float fMeshW, float fMeshH, 
	float fPosX, float fPosY )
{
	hgeDistortionMesh* pMesh;
	HTEXTURE tex = hge->Texture_Load( szTexPath );

	if ( !tex ) 
	{
		return false;
	}
	pMesh = new hgeDistortionMesh( MESHCOL, MESHROW );

	m_meshArray[ m_iMesh ] = pMesh;
	m_texArray[ m_iMesh ] = tex;
	m_meshPosArray[ m_iMesh ] = Point( fPosX, fPosY );

	pMesh->SetTexture(tex);
	pMesh->SetTextureRect(fMeshX,fMeshY,fMeshW,fMeshH);
	pMesh->SetBlendMode( BLEND_DEFAULT_Z );
	//pMesh->Clear(0xFF000000);

	m_iMesh++;
	return true;
}


//---------------------------------------------------XEtLder-------------------------------------------

XEtLder::XEtLder(const char* szFileName, float fStartX, float fStartY, float fWidth, float fHeight, 
	int iFrame /* = 4 */, EENTITYS eType /*= HGE_DISTORTIONMESH*/):
XEntity(szFileName,fStartX,fStartY,fWidth,fHeight,iFrame, eType, 4, 4 )
{
	m_iCols = 8;
	m_iRows = 8;

	m_fCellW = fWidth/(m_iCols - 1);
	m_fCellH = fHeight*iFrame/(m_iRows - 1);

	m_partTex =  hge->Texture_Load( "media/leader02.png" );
	m_pParticialMesh = new hgeDistortionMesh( 2, 2 );

	m_pParticialMesh->SetTexture(m_partTex);
	m_pParticialMesh->SetTextureRect(0.0f,0.0f,22.0f,22.0f);
	m_pParticialMesh->SetBlendMode( BLEND_DEFAULT_Z );

	m_fRot = 0.34*PI;
}

XEtLder::~XEtLder()
{

}

bool XEtLder::update( ENTITYBEHAVIOR eBe )
{
	float fRot = m_fRot;
	float fT = m_pScene->getTotalTime();
	float fDx, fDy;



	static float s_fDGradiantL = 0.0f;
	static float s_fDGradiantR = 0.0f;
	static bool s_bRock = false;
	switch( eBe )
	{
	case LEFT_KEY:
		{
			s_bRock = true;
		}
		break;
	case RIGHT_KEY:
		break;
	case UP_KEY:
		{
			if ( !m_pScene->isInWalkingZone()  )
			{
				fPos[1] += 2*fStep;
				m_pScene->update( SCENE_SCROLL_UP);
				if ( m_pScene->getCurrentSceneStatus() == MOVING )
				{
					fPos[1] += SCROLL_INTERVAL_Y;	
					setPos( getPosX(),getPosY() + SCROLL_INTERVAL_Y );
				}
				
			}
		}
	case IDLE:
		break;
	case DEFAULT:
		{
			static float sfdR = 0.5*PI;
			float fA = fRot*sin( sfdR );
			sfdR += 0.003*PI;	
			for( int i=1; i<m_iRows-1; i++)
				for( int j=1; j<m_iCols-1; j++)
				{

					fDx=sinf(fA)*(i*m_fCellH)+cosf(fA)*(j*m_fCellW-m_fCellW/2)+cosf(fT*3+(i+j)/2)*3;
					fDy=cosf(fA)*(i*m_fCellH)-sinf(fA)*(j*m_fCellW-m_fCellW/2)+sinf(fT*3+(i+j)/2)*3;

					

					if ( !s_bRock )
					{
						m_pParticialMesh->SetDisplacement( j, i, fDx,fDy,HGEDISP_NODE );
						m_fPartPosX = getPosX() + 13.0f;
						m_fPartPosY =  getPosY() +42.0f;         //相对载体的偏移
						m_pSr.pMesh->SetDisplacement( j, i, fDx,fDy,
						HGEDISP_NODE );
					}
					else
					{
							m_pParticialMesh->SetDisplacement(j,i,cosf(fT*3+(i+j)/2)*5,sinf(fT*3+(i+j)/2)*5,
							HGEDISP_NODE);
					}
				}

				if ( s_bRock )
					frameImage();
		}
		break;
	default:
		break;
	}

	m_fRot = fRot;

	return false;
}

bool XEtLder::render()
{
	m_pSr.pMesh->Render( getPosX(), getPosY() );
	m_pParticialMesh->Render( m_fPartPosX, m_fPartPosY );
	return false;
}

ENTITYBEHAVIOR XEtLder::onBehaviors( int iBes )
{
	if ( iBes & UP_KEY)
		return UP_KEY;
	if ( iBes & LEFT_KEY )
		return LEFT_KEY;
	if ( iBes & RIGHT_KEY )
		return RIGHT_KEY;

	if (  iBes & DOWN_KEY )
		return DOWN_KEY;
	if ( iBes & SCENE_SCROLL_UP )
		return IDLE;
	if ( iBes & SCENE_SCROLL_DOWN )
		return IDLE;
	if ( iBes & SCENE_SCROLL_LEFT )
		return IDLE;
	if( iBes & SCENE_SCROLL_RIGHT )
		return IDLE;

	return DEFAULT;
}

bool XEtLder::frameImage()
{
	static float s_fT[8] = { 0.0f };
	float dfT = m_pScene->getElapseTime();
	s_fT[0] += dfT;
	s_fT[1] += dfT;
	s_fT[3] += dfT;
	s_fT[4] += dfT;
	s_fT[5] += dfT;
	s_fT[6] += dfT;
	s_fT[7] += dfT;
	
	float fDy = -0.03f;
	static float s_fStartY = m_fPartPosY;
	static float s_fStartX = m_fPartPosX;
	
	float fScrollVar = 0.12f;

	if ( m_eStatus == OUTSCENE)
	{
		m_fPartPosX = fPos[0];
		m_fPartPosY = fPos[1];
		fScrollVar = 0.02f;
	}

	if ( s_fT[0] > 0.1f && m_eStatus != OUTSCENE )
	{
		m_fPartPosY = s_fStartY + 150.0f*sinf(s_fT[2]/2);
		m_fPartPosX = s_fStartX + 70.0f*cosf(0.5*PI + s_fT[2]);

		s_fT[2] += fDy;
		s_fT[0] = 0.0f;
	}
	
	if ( m_fPartPosY < WINDOW_H/3 && s_fT[1] > fScrollVar )
	{
		m_pScene->update( SCENE_SCROLL_UP );
		if ( m_pScene->getCurrentSceneStatus() == MOVING )
		{
			if ( m_eStatus == OUTSCENE )
				m_fPartPosY += SCROLL_INTERVAL_Y;
			
			setPos( getPosX(), getPosY() + SCROLL_INTERVAL_Y );
		}
		s_fT[1] = 0.0f;
	}
	
	

	if ( m_fPartPosY > WINDOW_H/3*2 && s_fT[3] > fScrollVar && hge->Input_GetKeyState(HGEK_DOWN) )
	{
		m_pScene->update( SCENE_SCROLL_DOWN );
		if ( m_pScene->getCurrentSceneStatus() == MOVING )
		{ 
			if ( m_eStatus == OUTSCENE )
				m_fPartPosY -= SCROLL_INTERVAL_Y;
			setPos( getPosX(),getPosY() - SCROLL_INTERVAL_Y );
		}

		s_fT[3] = 0.0f;
	}

	if ( m_fPartPosX > WINDOW_W/3*2 && s_fT[5] > fScrollVar )
	{
		m_pScene->update( SCENE_SCROLL_RIGHT );
		if ( m_pScene->getCurrentSceneStatus() == MOVING )
		{ 
			if ( m_eStatus == OUTSCENE )
				m_fPartPosX -= SCROLL_INTERVAL_X;
			setPos( getPosX() - SCROLL_INTERVAL_X,getPosY() );
		}
		s_fT[5] = 0.0f;
	}

	if ( m_fPartPosX < WINDOW_W/3 && s_fT[6] > fScrollVar )
	{
		m_pScene->update( SCENE_SCROLL_LEFT );
		if ( m_pScene->getCurrentSceneStatus() == MOVING )
		{ 
			if ( m_eStatus == OUTSCENE )
				m_fPartPosX += SCROLL_INTERVAL_X;
			setPos( getPosX() + SCROLL_INTERVAL_X,getPosY()  );
		}
		s_fT[6] = 0.0f;
	}

	if ( m_eStatus == OUTSCENE)
	{
		fPos[0]= m_fPartPosX;
		fPos[1] = m_fPartPosY;
	}

	if ( m_fPartPosY > WINDOW_H/4*3 && m_eStatus == INSCENE )
	{
		m_eStatus = LEAVING_SCENE;
	}

	if ( m_eStatus == LEAVING_SCENE )
	{
		if ( s_fT[4] > 0.1f )
		{
			bool bTransparent = true;
			for( int i=0; i<m_iRows-1; i++)
				for( int j=0; j<m_iCols-1; j++)
				{
					int iDelta = GETA(m_pParticialMesh->GetColor( j,i )) - 30;
					if ( iDelta < 0 )
						iDelta = 0;

					m_pParticialMesh->SetColor( j,i, SETA(m_pParticialMesh->GetColor( j,i ), 
						iDelta ));
					int iAlpha = GETA(m_pParticialMesh->GetColor( j,i ));
					if ( iDelta > 0 )
					{
						bTransparent = false;
					}
					if ( iDelta == 0 && bTransparent)
					{
						bTransparent = true;
					}	
				}
			s_fT[4] = 0.0f;	

			if ( bTransparent )
			{
				m_eStatus = OUTSCENE;
				fPos[0] = m_fPartPosX;
				fPos[1] = m_fPartPosY;
				g_bLeaderInit = true;
			}
		}

	}
	return false;
}
