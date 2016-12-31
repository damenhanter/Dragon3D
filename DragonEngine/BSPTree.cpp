#include "StdAfx.h"
#include "BSPTree.h"

CBSPTree::CBSPTree(void)
{

}

CBSPTree::~CBSPTree(void)
{
}

void CBSPTree::IntersectLines(float x0, float y0, float x1, float y1,
							  float x2, float y2, float x3, float y3,
							  float &xi, float &yi)
{
	//�������ֱ�ߵĽ��㣬���ÿ���ķ����
	//ֱ�߷���:(m)*x+(-1)*y=(m*x0-y0)

	float a1,b1,c1,a2,b2,c2,det_inv,k1,k2;

	//����б��
	if ((x1-x0)!=0)
	{
		k1=(y1-y0)/(x1-x0);
	}
	else
	{
		k1=(float)1.0E+20; //��ʾб�������
	}

	if ((x3-x2)!=0)
	{
		k2=(y3-y2)/(x3-x2);
	}
	else
	{
		k2=(float)1.0E+20;
	}
	
	a1=k1;
	a2=k2;
	b1=-1;
	b2=-1;
	c1=(y0-k1*x0);
	c2=(y2-k2*x2);

	det_inv=1/(a1*b2-a2*b1);

	xi=((b1*c2-b2*c1)*det_inv);
	yi=((a2*c1-a1*c2)*det_inv);

}

void CBSPTree::SetUp(LPBSPNODE* tempRoot)
{
	static LPBSPNODE next;  // ָ��Ҫ�������һ��ǽ��
	static LPBSPNODE front;
	static LPBSPNODE back;
	static LPBSPNODE temp;

	static float dot_wall_1,dot_wall_2;
	static float wall_x0,wall_y0,wall_z0,wall_x1,wall_y1,wall_z1;
	static float pp_x0,pp_y0,pp_z0,pp_x1,pp_y1,pp_z1,xi,zi;

	static CVector4 v1,v2;  //�ӷָ��浽ǰ�������
	static BOOL frontFlag=FALSE,backFlag=FALSE;

	if ((*tempRoot)==NULL)
	{
		return;
	}
	
	//1.��⵱ǰ(*tempRoot)�Ƿ�Ϊ�գ����������ȡ�ָ���Ķ���
	//��(*tempRoot)�ڵ������ǽ�������ָ���
	next=(*tempRoot)->link;
	(*tempRoot)->link=NULL;

	pp_x0=(*tempRoot)->wall.vlist[0].x;
	pp_y0=(*tempRoot)->wall.vlist[0].y;
	pp_z0=(*tempRoot)->wall.vlist[0].z;
	pp_x1=(*tempRoot)->wall.vlist[1].x;
	pp_y1=(*tempRoot)->wall.vlist[1].y;
	pp_z1=(*tempRoot)->wall.vlist[1].z;

	//2.����ǽ�������е�һ��ǽ����������㣬ʹ�õ����ȷ��ǽ�洦�ڷָ����λ��
	while(next)
	{
		//����ָ����ϵĵ㵽ǽ���������������
		v1.CreateVector((*tempRoot)->wall.vlist[0].v,next->wall.vlist[0].v);
		v2.CreateVector((*tempRoot)->wall.vlist[0].v,next->wall.vlist[1].v);
		//������������������ǽ�淢�ֵĵ��
		//����������������Ǹ����ж�ǽ���ϵĵ�λ���Ǹ���ռ�
		dot_wall_1=v1.DotProduct((*tempRoot)->wall.n);
		dot_wall_2=v2.DotProduct((*tempRoot)->wall.n);

		//3.�����ö˵���������

		//1:�ָ����ǽ�湲��һ���˵�
		frontFlag=backFlag=FALSE;

		if ((*tempRoot)->wall.vlist[0].v==next->wall.vlist[0].v)
		{
			if (dot_wall_2>0)
			{
				frontFlag=TRUE;
			}
			else
			{
				backFlag=TRUE;
			}
		}
		else if ((*tempRoot)->wall.vlist[0].v==next->wall.vlist[1].v)
		{
			if (dot_wall_1>0)
			{
				frontFlag=TRUE;
			}
			else
			{
				backFlag=TRUE;
			}
		}
		else if ((*tempRoot)->wall.vlist[1].v==next->wall.vlist[0].v)
		{
			if (dot_wall_2>0)
			{
				frontFlag=TRUE;
			}
			else
			{
				backFlag=TRUE;
			}
		}
		else if ((*tempRoot)->wall.vlist[1].v==next->wall.vlist[1].v)
		{
			if (dot_wall_1>0)
			{
				frontFlag=TRUE;
			}
			else
			{
				backFlag=TRUE;
			}
		}
		
		//4.���ݵ���ķ����ж�ǽ���Ƿ��ڷָ����ǰ�棬����ǣ�������뵽front����
		//case 1: ��������ķ�����ͬ������frontFlag��backFlag������
		if ((dot_wall_1>=0&&dot_wall_2>=0)||frontFlag)
		{
			//��ǽ�����front������
			if ((*tempRoot)->front==NULL)
			{
				//���ǵ�һ���ڵ�
				(*tempRoot)->front=next;
				next=next->link;
				front=(*tempRoot)->front;
				front->link=NULL;
			}
			else
			{
				//���ǵ�n���ڵ�
				front->link=next;
				next=next->link;
				front=front->link;
				front->link=NULL;
			}
		}

		//ǽ��λ�ڷָ������
		else if ((dot_wall_1<0&&dot_wall_2<0)||backFlag)
		{
			if ((*tempRoot)->back==NULL)
			{
				//���ǵ�һ���ڵ�
				(*tempRoot)->back=next;
				next=next->link;
				back=(*tempRoot)->back;
				back->link=NULL;
			}
			else
			{
				//���ǵ�N���ڵ�
				back->link=next;
				next=next->link;
				back=back->link;
				back->link=NULL;
			}
		}
		//case 2:��������෴������ָ�ǽ��
		else if ((dot_wall_1<0&&dot_wall_2>=0)||
			(dot_wall_1>=0&&dot_wall_2<0))
		{
			wall_x0=next->wall.vlist[0].x;
			wall_y0=next->wall.vlist[0].y;
			wall_z0=next->wall.vlist[0].z;
			wall_x1=next->wall.vlist[1].x;
			wall_y1=next->wall.vlist[1].y;
			wall_z1=next->wall.vlist[1].z;
			//����ǽ��ͷָ���Ľ���
			IntersectLines(wall_x0,wall_z0,wall_x1,wall_z1,
				pp_x0,pp_z0,pp_x1,pp_z1,xi,zi);
			//��Ҫ�ָ�ǽ�棬�õ�������ǽ��
			//Ȼ�����ǲ���front��back������
			temp=new BSPNODE();

			temp->front=NULL;
			temp->back=NULL;
			temp->link=NULL;

			//����η��߲���
			temp->wall.n=next->wall.n;
			temp->wall.nlength=next->wall.nlength;
			//�������ɫ����
			temp->wall.color=next->wall.color;
			//���ʲ���
			temp->wall.mati=next->wall.mati;
			//������
			temp->wall.ptrTexture=next->wall.ptrTexture;
			//���Բ���
			temp->wall.attr=next->wall.attr;
			//״̬����
			temp->wall.state=next->wall.state;
			//�޸�ID
			temp->id=next->id+WALL_SPLIT_ID;
			//���㶥������
			for (int i=0;i<4;i++)
			{
				temp->wall.vlist[i].x=next->wall.vlist[i].x;
				temp->wall.vlist[i].y=next->wall.vlist[i].y;
				temp->wall.vlist[i].z=next->wall.vlist[i].z;
				temp->wall.vlist[i].w=1;

				temp->wall.vlist[i].attr=next->wall.vlist[i].attr;
				temp->wall.vlist[i].n=next->wall.vlist[i].n;
				temp->wall.vlist[i].t=next->wall.vlist[i].t;
			}
			//������1��2����������Ϊ�������꣬y���겻��
			temp->wall.vlist[1].x=xi;
			temp->wall.vlist[1].z=zi;

			temp->wall.vlist[2].x=xi;
			temp->wall.vlist[2].z=zi;
			//�����µ�ǽ��
			if (dot_wall_1>=0)
			{
				if ((*tempRoot)->front==NULL)
				{
					//ͷ��㣿
					(*tempRoot)->front=temp;
					front=(*tempRoot)->front;
					front->link=NULL;
				}
				else
				{
					//��n���ڵ�
					front->link=temp;
					front=front->link;
					front->link=NULL;
				}
			}
			else if (dot_wall_1<0)
			{
				if ((*tempRoot)->back==NULL)
				{
					//ͷ��㣿
					(*tempRoot)->back=temp;
					back=(*tempRoot)->back;
					back->link=NULL;
				}
				else
				{
					//��n���ڵ�
					back->link=temp;
					back=back->link;
					back->link=NULL;
				}
			}

			//����ڶ�����ǽ��
			temp=new BSPNODE;

			temp->front=NULL;
			temp->back=NULL;
			temp->link=NULL;

			//����η��߲���
			temp->wall.n=next->wall.n;
			temp->wall.nlength=next->wall.nlength;
			//�������ɫ����
			temp->wall.color=next->wall.color;
			//���ʲ���
			temp->wall.mati=next->wall.mati;
			//������
			temp->wall.ptrTexture=next->wall.ptrTexture;
			//���Բ���
			temp->wall.attr=next->wall.attr;
			//״̬����
			temp->wall.state=next->wall.state;
			//�޸�ID
			temp->id=next->id+WALL_SPLIT_ID;
			//���㶥������
			for (int i=0;i<4;i++)
			{
				temp->wall.vlist[i].x=next->wall.vlist[i].x;
				temp->wall.vlist[i].y=next->wall.vlist[i].y;
				temp->wall.vlist[i].z=next->wall.vlist[i].z;
				temp->wall.vlist[i].w=1;

				temp->wall.vlist[i].attr=next->wall.vlist[i].attr;
				temp->wall.vlist[i].n=next->wall.vlist[i].n;
				temp->wall.vlist[i].t=next->wall.vlist[i].t;
			}
			//������0��3����������Ϊ�������꣬y���겻��
			temp->wall.vlist[0].x=xi;
			temp->wall.vlist[0].z=zi;

			temp->wall.vlist[3].x=xi;
			temp->wall.vlist[3].z=zi;
			//�����µ�ǽ��
			if (dot_wall_2>=0)
			{
				if ((*tempRoot)->front==NULL)
				{
					//ͷ��㣿
					(*tempRoot)->front=temp;
					front=(*tempRoot)->front;
					front->link=NULL;
				}
				else
				{
					//��n���ڵ�
					front->link=temp;
					front=front->link;
					front->link=NULL;
				}
			}
			else if (dot_wall_2<0)
			{
				if ((*tempRoot)->back==NULL)
				{
					//ͷ��㣿
					(*tempRoot)->back=temp;
					back=(*tempRoot)->back;
					back->link=NULL;
				}
				else
				{
					//��n���ڵ�
					back->link=temp;
					back=back->link;
					back->link=NULL;
				}
			}
			temp=next;
			next=next->link;
			delete temp;
		}
	}
	SetUp(&((*tempRoot))->front);
	SetUp(&((*tempRoot))->back);
}

void CBSPTree::TransformBSP(LPBSPNODE root,CMatrix4x4 &mt, int coord_select)
{
	if (root==NULL)
	{
		return;
	}
	TransformBSP(root->back,mt,coord_select);
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
		{
			for (int vertex=0;vertex<4;vertex++)
			{
				root->wall.vlist[vertex].v=root->wall.vlist[vertex].v*mt;
				if (root->wall.vlist[vertex].attr & VERTEX_ATTR_NORMAL)
				{
					root->wall.vlist[vertex].n=root->wall.vlist[vertex].n*mt;
				}
			}
		}break;
	case TRANSFORM_TRANS_ONLY:
		{
			for (int vertex=0;vertex<4;vertex++)
			{
				root->wall.tvlist[vertex].v=root->wall.tvlist[vertex].v*mt;
				if (root->wall.tvlist[vertex].attr & VERTEX_ATTR_NORMAL)
				{
					root->wall.tvlist[vertex].n=root->wall.tvlist[vertex].n*mt;
				}
			}
		}break;
	case TRANSFORM_LOCAL_TO_TRANS:
		{
			for (int vertex=0;vertex<4;vertex++)
			{
				root->wall.tvlist[vertex].v=root->wall.vlist[vertex].v*mt;
				if (root->wall.vlist[vertex].attr & VERTEX_ATTR_NORMAL)
				{
					root->wall.tvlist[vertex].n=root->wall.vlist[vertex].n*mt;
				}
			}
		}break;
	default:
		break;
	}
	TransformBSP(root->front,mt,coord_select);
}

void CBSPTree::DeleteBSP(LPBSPNODE* root)
{
	LPBSPNODE temp;
	if ((*root)==NULL)
	{
		return;
	}
	DeleteBSP(&(*root)->back);
	temp=(*root)->front;
	delete (*root);
	DeleteBSP(&temp);
}

void BHVTreeSetUp(LPBHVNODE bhvTree,   //������bhv��
				  LPOBJCONTAINER bhvObjects,  //ָ�򳡾��е����������ָ��
				  int numObjects,    //�������������Ŀ
				  int level,    //bhv�����
				  int numDivisions, //��ÿ���Ὣ�ڵ�ָ�ɶ��ٷ�
				  int universeRadius  //��Ϸ����Ĵ�С
				  )
{
	//level==0����������
	if (level==0)
	{
		bhvTree->pos.x=0;
		bhvTree->pos.y=0;
		bhvTree->pos.z=0;
		bhvTree->pos.w=1;
		
		//���ڵ�뾶����Ϊ��Ϸ����İ뾶
		bhvTree->radius.x=universeRadius;
		bhvTree->radius.y=universeRadius;
		bhvTree->radius.z=universeRadius;
		bhvTree->radius.w=1;

		//�������ڵ㣬�������������ýڵ���
		for (int i=0;i<numObjects;i++)
		{
			if (!(bhvObjects[i].state & OBJMODEL4DV1_STATE_CULLED))
			{
				bhvTree->objects[bhvTree->numObjects++]=(LPOBJCONTAINER)&bhvObjects[i];
			}
		}

		bhvTree->state=OBJMODEL4DV1_STATE_ACTIVE;
		bhvTree->attr=0;

		//������ָ���ӽڵ��ָ�붼����ΪNULL
		for (int ilink=0;ilink<MAX_BHV_PER_NODE;ilink++)
		{
			bhvTree->links[ilink]=NULL;
		}

		bhvTree->numObjects=numObjects;
		BHVTreeSetUp(bhvTree,bhvObjects,numObjects,1,numDivisions,universeRadius);
	}
	else
	{
		//������״̬
		if (bhvTree->numObjects<=MIN_OBJECTS_PER_BHV_CELL)
		{
			return;
		}
		
		//�����ӽڵ�
		//���뽫��ǰ�ڵ㻮�ֳɶ�����ֽ�
		//Ȼ��������뵽ÿ���ӽڵ��У��ٵݹ���ú������Դ���BHV����

		//������ʱ3D��Ԫ��
		BHVCELL cells[MAX_BHV_CELL_DIVISIONS][MAX_BHV_CELL_DIVISIONS][MAX_BHV_CELL_DIVISIONS];
		
		//���ݰ뾶�������ҳ���Χ���ԭ��
		int x0=bhvTree->pos.x-bhvTree->radius.x;
		int y0=bhvTree->pos.y-bhvTree->radius.y;
		int z0=bhvTree->pos.z-bhvTree->radius.z;

		//���㵥Ԫ����x,y,z��Ĵ�С
		float cell_size_x=2*bhvTree->radius.x/(float)numDivisions;
		float cell_size_y=2*bhvTree->radius.y/(float)numDivisions;
		float cell_size_z=2*bhvTree->radius.z/(float)numDivisions;

		int cell_x,cell_y,cell_z; //���ڴ洢��Ԫ����3D�����е�λ��

		memset(cells,0,sizeof(cells));

		//��ÿ���Ὣ�ռ�ֳ�numdivisions�ȷ�
		//ȷ�����������λ���ĸ���Ԫ
		for (int i=0;i<bhvTree->numObjects;i++)
		{
			//��������λ���ĸ���Ԫ
			cell_x=(bhvTree->objects[i]->pos.x-x0)/cell_size_x;
			cell_y=(bhvTree->objects[i]->pos.y-y0)/cell_size_y;
			cell_z=(bhvTree->objects[i]->pos.z-z0)/cell_size_z;

			cells[cell_x][cell_y][cell_z].objList[cells[cell_x][cell_y][cell_z].numObjects]=
				bhvTree->objects[i];
			if (++cells[cell_x][cell_y][cell_z].numObjects>=MAX_OBJECTS_PER_BHV_CELL)
			{
				cells[cell_x][cell_y][cell_z].numObjects=MAX_OBJECTS_PER_BHV_CELL-1;
			}
		}

		//���˵�Ԫ���Ϊÿ����Ϊ�յĵ�Ԫ�񴴽�һ��BHV�ڵ�
		for (int icell_x=0; icell_x<numDivisions ; icell_x++)
		{
			for (int icell_y=0; icell_y<numDivisions; icell_y++)
			{
				for (int icell_z=0; icell_z<numDivisions;icell_z++)
				{
					//��Ԫ������������
					if (cells[icell_x][icell_y][icell_z].numObjects>0)
					{
						//����һ���ڵ�,����ָ��ָ����
						bhvTree->links[bhvTree->numChildren]=new BHVNODE;
						memset(bhvTree->links[bhvTree->numChildren],0,sizeof(BHVNODE));

						//���ýڵ�
						LPBHVNODE currNode=bhvTree->links[bhvTree->numChildren];
						
						currNode->pos.x=(icell_x*cell_size_x+cell_size_x/2)+x0;
						currNode->pos.y=(icell_y*cell_size_y+cell_size_y/2)+y0;
						currNode->pos.z=(icell_z*cell_size_z+cell_size_z/2)+z0;
						currNode->pos.w=1;

						currNode->radius.x=cell_size_x/2;
						currNode->radius.y=cell_size_y/2;
						currNode->radius.z=cell_size_z/2;
						currNode->radius.w=1;

						currNode->numObjects=cells[icell_x][icell_y][icell_z].numObjects;
						currNode->numChildren=0;

						currNode->state=OBJMODEL4DV1_STATE_ACTIVE;
						currNode->attr=0;

						//��������뵽�ڵ�������б���
						for (int i=0;i<currNode->numObjects;i++)
						{
							currNode->objects[i]=cells[icell_x][icell_y][icell_z].objList[i];
						}

						bhvTree->numChildren++;
					}
				}
			}
		}
		//Ϊÿһ���ڵ㴴��һ��BHV����
		for (int i=0;i<bhvTree->numChildren;i++)
		{
			BHVTreeSetUp(bhvTree->links[i],NULL,NULL,level+1,numDivisions,universeRadius);
		}
	}
}

void ResetBHVTree(LPBHVNODE bhvTree)
{
	for (int i=0;i<bhvTree->numObjects;i++)
	{
		RESET_BIT(bhvTree->objects[i]->state,OBJMODEL4DV1_STATE_CULLED);
	}
}