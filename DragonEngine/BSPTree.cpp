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
	//求解两条直线的交点，采用克莱姆法则
	//直线方程:(m)*x+(-1)*y=(m*x0-y0)

	float a1,b1,c1,a2,b2,c2,det_inv,k1,k2;

	//计算斜率
	if ((x1-x0)!=0)
	{
		k1=(y1-y0)/(x1-x0);
	}
	else
	{
		k1=(float)1.0E+20; //表示斜率无穷大
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
	static LPBSPNODE next;  // 指向要处理的下一个墙面
	static LPBSPNODE front;
	static LPBSPNODE back;
	static LPBSPNODE temp;

	static float dot_wall_1,dot_wall_2;
	static float wall_x0,wall_y0,wall_z0,wall_x1,wall_y1,wall_z1;
	static float pp_x0,pp_y0,pp_z0,pp_x1,pp_y1,pp_z1,xi,zi;

	static CVector4 v1,v2;  //从分割面到前面的向量
	static BOOL frontFlag=FALSE,backFlag=FALSE;

	if ((*tempRoot)==NULL)
	{
		return;
	}
	
	//1.检测当前(*tempRoot)是否为空，如果不是提取分割面的顶点
	//将(*tempRoot)节点包含的墙面用作分割面
	next=(*tempRoot)->link;
	(*tempRoot)->link=NULL;

	pp_x0=(*tempRoot)->wall.vlist[0].x;
	pp_y0=(*tempRoot)->wall.vlist[0].y;
	pp_z0=(*tempRoot)->wall.vlist[0].z;
	pp_x1=(*tempRoot)->wall.vlist[1].x;
	pp_y1=(*tempRoot)->wall.vlist[1].y;
	pp_z1=(*tempRoot)->wall.vlist[1].z;

	//2.根据墙面链表中第一个墙面的两个顶点，使用点击来确定墙面处于分割面的位置
	while(next)
	{
		//计算分割面上的点到墙面上两个点的向量
		v1.CreateVector((*tempRoot)->wall.vlist[0].v,next->wall.vlist[0].v);
		v2.CreateVector((*tempRoot)->wall.vlist[0].v,next->wall.vlist[1].v);
		//计算上述两个向量与墙面发现的点积
		//并分析点积是正还是负来判断墙面上的点位于那个半空间
		dot_wall_1=v1.DotProduct((*tempRoot)->wall.n);
		dot_wall_2=v2.DotProduct((*tempRoot)->wall.n);

		//3.处理共用端点的特殊情况

		//1:分割面和墙面共用一个端点
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
		
		//4.根据点积的符号判断墙面是否在分割面的前面，如果是，将其加入到front链表
		//case 1: 两个点积的符号相同，或者frontFlag或backFlag被设置
		if ((dot_wall_1>=0&&dot_wall_2>=0)||frontFlag)
		{
			//将墙面插入front链表中
			if ((*tempRoot)->front==NULL)
			{
				//这是第一个节点
				(*tempRoot)->front=next;
				next=next->link;
				front=(*tempRoot)->front;
				front->link=NULL;
			}
			else
			{
				//这是第n个节点
				front->link=next;
				next=next->link;
				front=front->link;
				front->link=NULL;
			}
		}

		//墙面位于分割面后面
		else if ((dot_wall_1<0&&dot_wall_2<0)||backFlag)
		{
			if ((*tempRoot)->back==NULL)
			{
				//这是第一个节点
				(*tempRoot)->back=next;
				next=next->link;
				back=(*tempRoot)->back;
				back->link=NULL;
			}
			else
			{
				//这是第N个节点
				back->link=next;
				next=next->link;
				back=back->link;
				back->link=NULL;
			}
		}
		//case 2:两个点积相反，必须分割墙面
		else if ((dot_wall_1<0&&dot_wall_2>=0)||
			(dot_wall_1>=0&&dot_wall_2<0))
		{
			wall_x0=next->wall.vlist[0].x;
			wall_y0=next->wall.vlist[0].y;
			wall_z0=next->wall.vlist[0].z;
			wall_x1=next->wall.vlist[1].x;
			wall_y1=next->wall.vlist[1].y;
			wall_z1=next->wall.vlist[1].z;
			//计算墙面和分割面的交点
			IntersectLines(wall_x0,wall_z0,wall_x1,wall_z1,
				pp_x0,pp_z0,pp_x1,pp_z1,xi,zi);
			//需要分隔墙面，得到两个新墙面
			//然后将它们插入front或back链表中
			temp=new BSPNODE();

			temp->front=NULL;
			temp->back=NULL;
			temp->link=NULL;

			//多边形法线不变
			temp->wall.n=next->wall.n;
			temp->wall.nlength=next->wall.nlength;
			//多边形颜色不变
			temp->wall.color=next->wall.color;
			//材质不变
			temp->wall.mati=next->wall.mati;
			//纹理不变
			temp->wall.ptrTexture=next->wall.ptrTexture;
			//属性不变
			temp->wall.attr=next->wall.attr;
			//状态不变
			temp->wall.state=next->wall.state;
			//修改ID
			temp->id=next->id+WALL_SPLIT_ID;
			//计算顶点坐标
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
			//将顶点1和2的坐标设置为交点坐标，y坐标不变
			temp->wall.vlist[1].x=xi;
			temp->wall.vlist[1].z=zi;

			temp->wall.vlist[2].x=xi;
			temp->wall.vlist[2].z=zi;
			//插入新的墙面
			if (dot_wall_1>=0)
			{
				if ((*tempRoot)->front==NULL)
				{
					//头结点？
					(*tempRoot)->front=temp;
					front=(*tempRoot)->front;
					front->link=NULL;
				}
				else
				{
					//第n个节点
					front->link=temp;
					front=front->link;
					front->link=NULL;
				}
			}
			else if (dot_wall_1<0)
			{
				if ((*tempRoot)->back==NULL)
				{
					//头结点？
					(*tempRoot)->back=temp;
					back=(*tempRoot)->back;
					back->link=NULL;
				}
				else
				{
					//第n个节点
					back->link=temp;
					back=back->link;
					back->link=NULL;
				}
			}

			//处理第二个新墙面
			temp=new BSPNODE;

			temp->front=NULL;
			temp->back=NULL;
			temp->link=NULL;

			//多边形法线不变
			temp->wall.n=next->wall.n;
			temp->wall.nlength=next->wall.nlength;
			//多边形颜色不变
			temp->wall.color=next->wall.color;
			//材质不变
			temp->wall.mati=next->wall.mati;
			//纹理不变
			temp->wall.ptrTexture=next->wall.ptrTexture;
			//属性不变
			temp->wall.attr=next->wall.attr;
			//状态不变
			temp->wall.state=next->wall.state;
			//修改ID
			temp->id=next->id+WALL_SPLIT_ID;
			//计算顶点坐标
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
			//将顶点0和3的坐标设置为交点坐标，y坐标不变
			temp->wall.vlist[0].x=xi;
			temp->wall.vlist[0].z=zi;

			temp->wall.vlist[3].x=xi;
			temp->wall.vlist[3].z=zi;
			//插入新的墙面
			if (dot_wall_2>=0)
			{
				if ((*tempRoot)->front==NULL)
				{
					//头结点？
					(*tempRoot)->front=temp;
					front=(*tempRoot)->front;
					front->link=NULL;
				}
				else
				{
					//第n个节点
					front->link=temp;
					front=front->link;
					front->link=NULL;
				}
			}
			else if (dot_wall_2<0)
			{
				if ((*tempRoot)->back==NULL)
				{
					//头结点？
					(*tempRoot)->back=temp;
					back=(*tempRoot)->back;
					back->link=NULL;
				}
				else
				{
					//第n个节点
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

void BHVTreeSetUp(LPBHVNODE bhvTree,   //创建的bhv树
				  LPOBJCONTAINER bhvObjects,  //指向场景中的物体数组的指针
				  int numObjects,    //场景中物体的数目
				  int level,    //bhv树层次
				  int numDivisions, //沿每个轴将节点分割成多少分
				  int universeRadius  //游戏世界的大小
				  )
{
	//level==0，创建树根
	if (level==0)
	{
		bhvTree->pos.x=0;
		bhvTree->pos.y=0;
		bhvTree->pos.z=0;
		bhvTree->pos.w=1;
		
		//将节点半径设置为游戏世界的半径
		bhvTree->radius.x=universeRadius;
		bhvTree->radius.y=universeRadius;
		bhvTree->radius.z=universeRadius;
		bhvTree->radius.w=1;

		//创建根节点，将所有物体加入该节点中
		for (int i=0;i<numObjects;i++)
		{
			if (!(bhvObjects[i].state & OBJMODEL4DV1_STATE_CULLED))
			{
				bhvTree->objects[bhvTree->numObjects++]=(LPOBJCONTAINER)&bhvObjects[i];
			}
		}

		bhvTree->state=OBJMODEL4DV1_STATE_ACTIVE;
		bhvTree->attr=0;

		//将所有指向子节点的指针都设置为NULL
		for (int ilink=0;ilink<MAX_BHV_PER_NODE;ilink++)
		{
			bhvTree->links[ilink]=NULL;
		}

		bhvTree->numObjects=numObjects;
		BHVTreeSetUp(bhvTree,bhvObjects,numObjects,1,numDivisions,universeRadius);
	}
	else
	{
		//检测结束状态
		if (bhvTree->numObjects<=MIN_OBJECTS_PER_BHV_CELL)
		{
			return;
		}
		
		//创建子节点
		//必须将当前节点划分成多个子字节
		//然后将物体插入到每个子节点中，再递归调用函数，以创建BHV子树

		//创建临时3D单元格
		BHVCELL cells[MAX_BHV_CELL_DIVISIONS][MAX_BHV_CELL_DIVISIONS][MAX_BHV_CELL_DIVISIONS];
		
		//根据半径和中心找出包围体的原点
		int x0=bhvTree->pos.x-bhvTree->radius.x;
		int y0=bhvTree->pos.y-bhvTree->radius.y;
		int z0=bhvTree->pos.z-bhvTree->radius.z;

		//计算单元格沿x,y,z轴的大小
		float cell_size_x=2*bhvTree->radius.x/(float)numDivisions;
		float cell_size_y=2*bhvTree->radius.y/(float)numDivisions;
		float cell_size_z=2*bhvTree->radius.z/(float)numDivisions;

		int cell_x,cell_y,cell_z; //用于存储单元格在3D阵列中的位置

		memset(cells,0,sizeof(cells));

		//沿每条轴将空间分成numdivisions等份
		//确定物体的中心位于哪个单元
		for (int i=0;i<bhvTree->numObjects;i++)
		{
			//计算物体位于哪个单元
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

		//有了单元格后，为每个不为空的单元格创建一个BHV节点
		for (int icell_x=0; icell_x<numDivisions ; icell_x++)
		{
			for (int icell_y=0; icell_y<numDivisions; icell_y++)
			{
				for (int icell_z=0; icell_z<numDivisions;icell_z++)
				{
					//单元格中有物体吗？
					if (cells[icell_x][icell_y][icell_z].numObjects>0)
					{
						//创建一个节点,并让指针指向它
						bhvTree->links[bhvTree->numChildren]=new BHVNODE;
						memset(bhvTree->links[bhvTree->numChildren],0,sizeof(BHVNODE));

						//设置节点
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

						//将物体插入到节点的物体列表中
						for (int i=0;i<currNode->numObjects;i++)
						{
							currNode->objects[i]=cells[icell_x][icell_y][icell_z].objList[i];
						}

						bhvTree->numChildren++;
					}
				}
			}
		}
		//为每一个节点创建一个BHV子树
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