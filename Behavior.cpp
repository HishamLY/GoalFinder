
#include "Behavior.h"
//#include "ColorFinder.h"
#include "Head.h"
#include "Walking.h"
#include "math.h"
#include "GoalFinder.h"

using namespace Robot;


int mark(0);
extern int countHeadSearchingCycle;

Behavior* Behavior::m_UniqueInstance = new Behavior();

Behavior::Behavior()
{
	State = NORMAL_STATE;
    GoBackState = TURNING;
	Tracker = BallTracker();
 	Follower = BallFollower();
	X_Moved = 0;//odometry, hanya digunakan dan diassign di aproachingball2
	Count = 0;//counter untuk transisi gawang
	PostPosition = 0;//status post yang didapatkan
	PanLastSeenBall = 0;//cek pan terakhir dapet bola
	CompassError = 0;//error compass terhadap setpoint
	CountToReady = 0;
	WaitUntilReady = false;
	CompassPost = 0;
}


void Behavior::Process(Point2D center, int Objectsize,Vector2<int> Distance)

{
	//untuk normal state dll yang nyari bola, maka ObjectSize diisi dengan logika 0 dan 1 untuk edge of field
	//untuk checkpost state dan goback yang nyari gawang, maka objectSize diisi dengan pembacaan nilai kompas
	ObjectPos = center;
	ObjectDistance = Distance;
	ObjectSize = Objectsize;

	switch(State)
	{
		case READY_STATE : ReadyState(); break;
		case KICK_OFF : KickOff(); break;
		case NORMAL_STATE : NormalState(); break;
		case DEFENSE_STATE : DefenseState(); break;
		case GOBACK_STATE : GobackState(); break;
		case CHECK_POST_STATE : checkPost(); break;
		case APROACHING_BALL : ProcessAproachingBall2(); break;
		case TURN_AROUND_BALL_STATE : turningBall(); break;
		case DRIBBLE_STATE : dribbleBall(); break;
		case KICK_STATE : KickState(); break;
		case STATIC_STATE : StaticState(); break;
		case FIRST_PENALTY_SEARCHING : FirstPenaltySearching(); break;
		case LOCALIZE_PENALTY_MARK : LocalizePenaltyMark(); break;
		case LOCALIZE_GOALPOST : LocalizeGoalPost(); break;
		case PASS_BALL_STATE : PassBall(); break;
		case PENALTY_KICK_STATE : PenaltyKick(); break;
	}
}
void Behavior::Process(Point2D center)
{
	ObjectPos = center;


	switch(State)
	{
		case READY_STATE : ReadyState(); break;
		case KICK_OFF : KickOff(); break;
		case NORMAL_STATE : NormalState(); break;
		case DEFENSE_STATE : DefenseState(); break;
		case GOBACK_STATE : GobackState(); break;
		case CHECK_POST_STATE : checkPost(); break;
		case APROACHING_BALL : ProcessAproachingBall2(); break;
		case TURN_AROUND_BALL_STATE : turningBall(); break;
		case DRIBBLE_STATE : dribbleBall(); break;
		case KICK_STATE : KickState(); break;
		case STATIC_STATE : StaticState(); break;
		case FIRST_PENALTY_SEARCHING : FirstPenaltySearching(); break;
		case LOCALIZE_PENALTY_MARK : LocalizePenaltyMark(); break;
		case LOCALIZE_GOALPOST : LocalizeGoalPost(); break;
		case PASS_BALL_STATE : PassBall(); break;
		case PENALTY_KICK_STATE : PenaltyKick(); break;
	}
}

void Behavior::CheckStatus()
{
	switch(State)
    {
        case READY_STATE : printf("READY STATE\n"); break;
        case NORMAL_STATE : printf("NORMAL STATE\n"); break;
        case CHECK_POST_STATE : printf("CHECK POST STATE\n"); break;
        case TURN_AROUND_BALL_STATE : printf("TURN AROUND BALL STATE\n"); break;
        case DRIBBLE_STATE : printf("DRIBBLE STATE\n"); break;
        case KICK_STATE : printf("KICK STATE\n"); break;
        case STATIC_STATE : printf("STATIC STATE\n"); break;
        case FIRST_PENALTY_SEARCHING : printf("FIRST PENALTY SEARCHING\n"); break;
        case LOCALIZE_PENALTY_MARK : printf("LOCALIZE PENALTY MARK\n"); break;
        case LOCALIZE_GOALPOST : printf("LOCALIZE GOAL POST\n"); break;
        case PASS_BALL_STATE : printf("PASS BALL STATE\n"); break;
        case PENALTY_KICK_STATE : printf("PENALTY KICK\n"); break;
	}
}

extern bool startKickOff; //difalsekan di kickstate, turningball, dribble ball


//========================BELUM DIGUNAKAN==========================================

void Behavior::LocalizePenaltyMark()
{
	Tracker.Process(ObjectPos);
  Follower.ProcessLocalizePenaltyMark(Tracker.ball_position);
}

void Behavior::LocalizeGoalPost()
{
	Tracker.ProcessCheckPost(ObjectPos);
	Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
}

void Behavior::FirstPenaltySearching()
{
	Tracker.Process(ObjectPos);
  Follower.ProcessFirstPenaltySearching(Tracker.ball_position);

	if(ObjectPos.X != -1 && ObjectPos.Y != -1)
		set_normal_state();
}

void Behavior::KickState()
{
	Tracker.Process(ObjectPos);
    Follower.ProcessKick(Tracker.ball_position);

	if(Action::GetInstance()->IsRunning() == 0)
    {
      Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
      Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);

        if(Follower.KickBall == 0)
        {
            if(Walking::GetInstance()->IsRunning() == 0)			
            Walking::GetInstance()->Start();
            CountToReady = 0;
        }
        else if(Follower.KickBall != 0)
        {
			startKickOff = false;
			usleep(8000);
	if(Walking::GetInstance()->IsRunning() == 1)	
      Walking::GetInstance()->Stop();
      if(CountToReady > 15)
			{
	  	  Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
        Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
		    if(Follower.KickBall == -1)
      	{
					usleep(8000);
          Action::GetInstance()->Start(12);//12);//123   // RIGHT KICK
          fprintf(stderr, "RightKick! \n");
        }
        else if(Follower.KickBall == 1)
        {
					usleep(8000);
          Action::GetInstance()->Start(13);//13);//117   // LEFT KICK
          fprintf(stderr, "LeftKick! \n");
        }
        else if(Follower.KickBall == -2)
        {
					usleep(8000);
          Action::GetInstance()->Start(205);   // right KICK with left leg
        	fprintf(stderr, "LeftKick! \n");
        }
        else if(Follower.KickBall == 2)
        {
        	usleep(8000);
          Action::GetInstance()->Start(207);   // LEFT KICK with right leg
          fprintf(stderr, "LeftKick! \n");
        }
			}
			else
				CountToReady++;
    }
  }
	else
	{
	if(Walking::GetInstance()->IsRunning() == 1)	
		Walking::GetInstance()->Stop();
		CountToReady= 0;
	}

	if(State == NORMAL_STATE)
	{
		WaitUntilReady = true;
		CountToReady = 0;
	}
}

void Behavior::PassBall()
{
//        Tracker.ProcessPenaltyKick(ObjectPos);
        //Follower.ProcessPenaltyKick(Tracker.ball_position);

        if(Action::GetInstance()->IsRunning() == 0)
        {
               // Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
               // Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);

                if(Follower.KickBall != 0)
                {
                    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
                    Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);

                    if(Follower.KickBall == -1)
                    {
                        Action::GetInstance()->Start(131);   // RIGHT KICK
                        //fprintf(stderr, "RightKick! \n");
                    }
                    else if(Follower.KickBall == 1)
                    {
                        Action::GetInstance()->Start(130);   // LEFT KICK
                        //fprintf(stderr, "LeftKick! \n");
                    }
                    else if(Follower.KickBall == 2)
                    {
                        Action::GetInstance()->Start(130);   // LEFT KICK
                        //fprintf(stderr, "LeftKick! \n");
                    }
                    else if(Follower.KickBall == 1)
                    {
                        Action::GetInstance()->Start(130);   // LEFT KICK
                        //fprintf(stderr, "LeftKick! \n");
                    }
                }
        }
}


void Behavior::StaticState()
{
	//printf("STATIC STATE ..................\n");
	Tracker.Process(ObjectPos);

	ReadyState();
}

void Behavior::ReadyState()
{
	//printf("READY STATE .....................\n");
	Follower.InitKickBallCount();
	Follower.initMember();
       	Behavior::GetInstance()->set_normal_state();//habis static, masuk ke normal state..tapi, karena kena pinalty, maka masuk ke static state lagi
	//tapi baru mengassign doang, belum dijalankan...nanti kalo udah dipanggil processnya, baru dijalanin...
        Walking::GetInstance()->stop_counting_step();

	Walking::GetInstance()->X_MOVE_AMPLITUDE = 0;
	Walking::GetInstance()->Y_MOVE_AMPLITUDE = 0;
	Walking::GetInstance()->A_MOVE_AMPLITUDE = 0;
	Walking::GetInstance()->A_MOVE_AIM_ON = false;

        Behavior::GetInstance()->CountToReady = 0;
        Behavior::GetInstance()->WaitUntilReady = true;

	countHeadSearchingCycle = 0;
}
//==============================================================================================================

void Behavior::NormalState()
{
	bool EdgeField = ObjectSize;
		Tracker.Process(ObjectPos);
	Follower.ProcessAproachingBall(Tracker.ball_position, ObjectDistance, EdgeField);

	if(State == CHECK_POST_STATE)
	{
		PanLastSeenBall = Head::GetInstance()->GetPanAngle();
		WaitUntilReady = true;
		CountToReady = 0;
		PostFound = 0;
		mark = 0;
	}
}


void Behavior::DefenseState()
{
	Tracker.Process(ObjectPos);
	Follower.ProcessDefense(Tracker.ball_position, ObjectDistance);
}

void Behavior::GobackState()
{
	double pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	double pan_range = Head::GetInstance()->GetLeftLimitAngle();
  double pan_percent = pan / pan_range;
	//printf("PostFound = %d \n", PostFound);
	//printf("GOBACKSTATE = %d\n", GoBackState);

	if(GoBackState == TURNING)		//sekarang muter
	{
if(Walking::GetInstance()->IsRunning() == 0)	
	Walking::GetInstance()->Start();
		Tracker.Process(ObjectPos);
		Follower.ProcessGoback(Tracker.ball_position, ObjectSize);	
		WaitUntilReady = true;
		CountToReady = 0;
	}
	else if(GoBackState == CHECK_OWN_GOAL)
	{
		if(WaitUntilReady)	//ALGORITMA MENCARI GAWANG
	  {
  		if(CountToReady > MaxCountToReady+7)
			//waktu yang diperlukan sampe tangannya turun
  	  {
  	  	Walking::GetInstance()->Stop();
				Head::GetInstance()->initSearchingPost(RIGHT);//posisi kepala sekarang di kiri, maka searching ke kanan
				WaitUntilReady = false;
				CountToReady = 0;
				countHeadSearchingCycle = 0;
			}
			else
			{
				CountToReady++;
				Head::GetInstance()->MoveByAngle(70, 50);//menggerakkan kepala ke posisi limit kiri dengan tilt 40
				//printf("please wait\n");
			}
		}
		else
		{
		if(Walking::GetInstance()->IsRunning() == 1)
  	  		Walking::GetInstance()->Stop();

				Tracker.ProcessCheckPost(ObjectPos);
			Follower.ProcessGoback(Tracker.ball_position, (int)CompassError);
			//printf("Center Head .X = %d Center Head.Y = %d\n", (int)GoalPercept::GetInstance()->CenterFoot.X, (int)GoalPercept::GetInstance()->CenterFoot.Y);
			//printf("ObjectDistance = %d \n", ObjectDistance.x);
			//printf("Count =%d\n", CountToReady);
			if((int)GoalPercept::GetInstance()->CenterFoot.Y == -1 && pan < Head::GetInstance()->m_RightLimit + 10)//ga nemu gawang, asumsi robot udah di belakang banget
			{
				//balik seperti searching ball;
				GoBackState = SEARCHING;
				WaitUntilReady = true;
				CountToReady = 0;				
			}
			else if((int)GoalPercept::GetInstance()->CenterFoot.Y > 120 && (int)GoalPercept::GetInstance()->CenterFoot.Y != -1)
			{

					GoBackState = SEARCHING;
					WaitUntilReady = true;
					CountToReady = 0;				

			}
			else if((int)GoalPercept::GetInstance()->CenterFoot.Y <= 120 && (int)GoalPercept::GetInstance()->CenterFoot.Y != -1)
			{

					GoBackState = GOING_BACK;
					WaitUntilReady = true;
					CountToReady = 0;				

			}
		}
	}
	else if(GoBackState == SEARCHING)
	{
	if(Walking::GetInstance()->IsRunning() == 0)	
	Walking::GetInstance()->Start();
		bool EdgeField = ObjectSize;
		Tracker.Process(ObjectPos);
	Follower.ProcessAproachingBall(Tracker.ball_position, ObjectDistance, EdgeField);
	}
	else if(GoBackState == GOING_BACK)
	{
if(Walking::GetInstance()->IsRunning() == 0)
		Walking::GetInstance()->Start();
		Tracker.Process(ObjectPos);
		Follower.ProcessGoback(Tracker.ball_position, ObjectSize);//ball position and edge of field
	
	}

}

void Behavior::KickOff()
{
	PanPostBoth = 15;
	//printf("kick off state\n");

	if(WaitUntilReady)
	{
		if(CountToReady > MaxCountToReady)
		{
			PanPostBoth = 0;
			sp = ObjectSize;//ini diisi set point untuk muter pertama kali, nilainya masih get angle8bit
			if (sp > 128)
      {
      	kompensator = sp - 128;
      }
      else
      {
        kompensator = 128 - sp;
      }
      
      CountToReady = 0;
      WaitUntilReady = false;
    }
    else
    {
    	CountToReady++;
   		PostPosition = 1;
   	}
   }
   else
   {

    cmps_now = ObjectSize;
    if (sp > 128)
    {
    	if (cmps_now >= kompensator)
      	cmps_error = cmps_now - kompensator - 128;
      else 
        cmps_error = 127 + cmps_now - kompensator;
    }
    else
    {
    	if (cmps_now >= 255 - kompensator)
        cmps_error = cmps_now + kompensator - 383;
      else
        cmps_error = cmps_now + kompensator -128;
    }
             
		//printf("Compass = %d \n", cmps_error);
  	Tracker.Process(ObjectPos);
		Follower.ProcessTurnAroundBall(Tracker.ball_position, PanPostBoth, cmps_error, PostPosition);
    if(State == NORMAL_STATE || State == DRIBBLE_STATE || State == CHECK_POST_STATE || State == KICK_STATE)
    {
	    Walking::GetInstance()->stop_counting_step();
      CountToReady = 0;
  		WaitUntilReady = true;
	}
}

}

void Behavior::checkPost2 () {
	//CheckPost implementasi dengan GoalFinder yang baru

	double pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	double pan_range = Head::GetInstance()->GetLeftLimitAngle();
 	double pan_percent = pan / pan_range;
	
	CompassError = -ObjectSize * 360 / 255;
	if (WaitUntilReady) {
		if (CountToReady > MaxCountReady + 3) {
			if(ObjectSize > 0) {
				ObjectSize -= 5;
			} else if(ObjectSize < 0) {
				ObjectSize += 5;
			}
			if(abs(CompassError) < 110)//110 derajat
			{
				GoalFinder::OppGoal = true;
				/*Head::GetInstance()->SetLimit(ObjectSize);//mengeset limit dari kepala berdasarkan pembacaan kompas. parameter objectsize merupakan besarnya pembacaan kompas
				Head::GetInstance()->MoveByAngle(Head::GetInstance()->m_LeftLimit, 60);//menggerakkan kepala ke posisi limit kiri*/
			}
			else
			{
				GoalFinder::OppGoal = false;
			    //Head::GetInstance()->m_LeftLimit = 90;
				/*Head::GetInstance()->SetLimit(-(ObjectSize*128/abs(ObjectSize)) + ObjectSize);//objectsize dalam 8bit				*/
			}	
		}			
	}
	
	

}

void Behavior::checkPost()
{

//PostFound = 0, masa saat robot mencari gawang hingga dapat pole pertama, 
//PostFound = 1, masa saat robot mencari pole kedua, 
//PsotFound = 2, robot telah selesai cari gawang, pindah state ke aproaching ball
//PostFound = 3, masa transisi.
//compass negatif kalo di kiri set point
//positif kalo di kanan set point-	

	GoBackState = TURNING;//GoBack State dijadikan ke turning

	double pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	double pan_range = Head::GetInstance()->GetLeftLimitAngle();
 	double pan_percent = pan / pan_range;

	CompassError = -ObjectSize * 360 / 255;

	//printf("PostFound = %d \n", PostFound);

	if(WaitUntilReady)	//ALGORITMA MENCARI GAWANG
  {
  	if(CountToReady > MaxCountToReady+3)
		//waktu yang diperlukan sampe tangannya turun
    {
	//digunakan untuk menentukan limit kepala, diberikan galat +- 5 derajat
    	if(ObjectSize > 0)
    		ObjectSize -= 5;
    	else if(ObjectSize < 0)
    		ObjectSize += 5;

			//printf("ObjectSize = %d \n", ObjectSize);
			if(abs(CompassError) < 110)//110 derajat
			{
				Head::GetInstance()->SetLimit(ObjectSize);//mengeset limit dari kepala berdasarkan pembacaan kompas. parameter objectsize merupakan besarnya pembacaan kompas
				Head::GetInstance()->MoveByAngle(Head::GetInstance()->m_LeftLimit, 60);//menggerakkan kepala ke posisi limit kiri
			}
			else
			{
			    //Head::GetInstance()->m_LeftLimit = 90;
				Head::GetInstance()->SetLimit(-(ObjectSize*128/abs(ObjectSize)) + ObjectSize);//objectsize dalam 8bit				
			}

			Head::GetInstance()->initSearchingPost(RIGHT);//posisi kepala sekarang di kiri, maka searching ke kanan

			//mengnolkan semua parameter 
			WaitUntilReady = false;
			PostFound = 0;
		    CountToReady = 0;
	        PanPostRight = 0;
		   	PanPostLeft = 0;
			PanPostBoth = 0;
			PostPosition = 0; //artinya di tengah
			countHeadSearchingCycle = 0;
		}
		else
		{
			CountToReady++;

			Follower.ProcessLocalizeGoalPost(Point2D(-1, -1));
			if(abs(CompassError) < 110)//110 derajat
			{
				Head::GetInstance()->SetLimit(ObjectSize);//mengeset limit dari kepala berdasarkan pembacaan kompas. parameter objectsize merupakan besarnya pembacaan kompas
				Head::GetInstance()->MoveByAngle(Head::GetInstance()->m_LeftLimit, 60);//menggerakkan kepala ke posisi limit kiri dengan tilt 40
	//			Head::GetInstance()->MoveByAngle(Head::GetInstance()->m_LeftLimit, 40);//menggerakkan kepala ke posisi limit kiri

			}
			else
			{
			Head::GetInstance()->SetLimit(-(ObjectSize*128/abs(ObjectSize)) + ObjectSize);//objectsize dalam 8bit				
			}

			
			Head::GetInstance()->initSearchingPost(RIGHT);//posisi kepala sekarang di kiri, maka searching ke kanan
		}
	}
	else
	{
		//printf("%d\n",CountToReady);
		//=================KALO ROBOT MENGHADAP KE GAWANG SENDIRI..SEKARANG TETEP DICEK GAWANGNYA
		CompassPost = CompassError;
		if(abs(CompassError) > 110)//kalo compass lebih dari 90 derajat, maka langsung turning ball, ga usah ngecek gawang lagi
		{
			//parameter sudut gawang dinolkan semua
			PanPostBoth = CompassError;
			PanPostLeft = 0;
			PanPostRight = 0;
			PostPosition = 0;
			WaitUntilReady = true;
                        CountToReady = 0;

                        PostFound = 0;

			//limit head diubah ke default lagi.
			Head::GetInstance()->m_LeftLimit = 70;
			Head::GetInstance()->m_RightLimit = -70;
                        set_aproaching_ball();
			return;
		} 
//		========================================================	




		if(CountToReady > MaxCountToReady)//countind dulu biar kepalanya steasy state
		{
			if(PostFound == 0)
			{
				if(ObjectPos.X ==  -1 || ObjectPos.Y == -1)
				{
				//kalo ga nemu warna kuning, maka tracking
					
					Tracker.ProcessCheckPost(ObjectPos);
					Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
					
					if( pan < Head::GetInstance()->m_RightLimit + 5) 			
					{
					//kalo udah sampe pojokan, belum dapet post sama sekali
						PanPostLeft = 0;
						PanPostRight = 0;
						PanPostBoth = 0;//diubah 210414
						PostFound = 2;
					}
				}
				else
				{
					if(GoalPercept::GetInstance()->Status == GoalPercept::POSSIBLE_RIGHT_POST
					|| GoalPercept::GetInstance()->Status == GoalPercept::POSSIBLE_LEFT_POST
					|| GoalPercept::GetInstance()->Status == GoalPercept::LEFT_POST
					|| GoalPercept::GetInstance()->Status == GoalPercept::RIGHT_POST)
					{
						if(ObjectPos.X > (Camera::WIDTH/2.0 - 30) && ObjectPos.X < (Camera::WIDTH/2.0 + 30))
						{
								PanPostLeft = pan - (ObjectPos.X - Camera::WIDTH/2.0) * (Camera::VIEW_H_ANGLE/Camera::WIDTH);
								PanPostRight = 0;
								PanPostBoth = 0;
								PostFound = 3;
								Head::GetInstance()->initSearchingPost(RIGHT);
						}
						else if	(pan > Head::GetInstance()->m_LeftLimit - 5 && ObjectPos.X <= (Camera::WIDTH/2.0 - 30))
						{
								PanPostLeft = 90 - (ObjectPos.X - Camera::WIDTH/2.0) * (Camera::VIEW_H_ANGLE/Camera::WIDTH);
								PanPostRight = 0;
								PanPostBoth = 0;
								PostFound = 3;
								Head::GetInstance()->initSearchingPost(RIGHT);
						}
						Tracker.ProcessCheckPost(ObjectPos);
						Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
						
					}
					else if(GoalPercept::GetInstance()->Status == GoalPercept::BOTH_POST)
					{
						if(ObjectPos.X > (Camera::WIDTH/2.0 - 30) && ObjectPos.X < (Camera::WIDTH/2.0 + 30))
						{
								PostFound = 2;
								PanPostLeft = 0;
								PanPostRight = 0;
								PanPostBoth = pan - (ObjectPos.X - Camera::WIDTH/2.0) * (Camera::VIEW_H_ANGLE/Camera::WIDTH);
						}
						Tracker.ProcessCheckPost(ObjectPos);
						Follower.ProcessLocalizeGoalPost(Tracker.ball_position);

					}	
				}
				printf("Post Found 0\n");
				printf("\nPanPostLeft = %lf \n", PanPostLeft);
				printf("\nPanPostRight = %lf \n", PanPostRight);
				printf("\nPanPostBoth = %lf \n", PanPostBoth);
			}
			else if(PostFound == 3)
			{
				if(pan < Head::GetInstance()->m_RightLimit + 5) 			
				{
					PostFound = 2;
				}
				else if(GoalPercept::GetInstance()->Status == GoalPercept::BOTH_POST)
				{
					if(ObjectPos.X > (Camera::WIDTH/2.0 - 30) && ObjectPos.X < (Camera::WIDTH/2.0 + 30))
					{
							PostFound = 2;
							PanPostLeft = 0;
							PanPostRight = 0;
							PanPostBoth = pan - (ObjectPos.X - Camera::WIDTH/2.0) * (Camera::VIEW_H_ANGLE/Camera::WIDTH);
					}
					Tracker.ProcessCheckPost(ObjectPos);
					Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
				}
				else if((GoalPercept::GetInstance()->CenterFoot.X != -1 
				|| GoalPercept::GetInstance()->CenterFoot.Y != -1  
				|| GoalPercept::GetInstance()->CenterHead.X != -1 
				|| GoalPercept::GetInstance()->CenterHead.Y != -1
				|| GoalPercept::GetInstance()->Center.X != -1 
				|| GoalPercept::GetInstance()->Center.Y != -1 )//kalo misal masih terlihat gawang
				&& (GoalPercept::GetInstance()->Center.X < Camera::WIDTH/2.0 + 30))
				{
						Head::GetInstance()->MoveSearchingPost();//20
				}
				else
				{
					if(Count > 5)
					{
						PostFound = 1;
						Count = 0;
					}
					else
						Count++;
				}
			}
			else if(PostFound == 1)
			{
			//kalo udah nemu salah satu tiang, maka bergerak lagi ke kanan
				if((ObjectPos.X ==  -1 || ObjectPos.Y == -1))
				{
				//kalo ga nemu warna kuning, dan jarak Object kurang dari Objectsebelumya + 40, maka tracking

					Tracker.ProcessCheckPost(ObjectPos);
					Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
					if(pan < Head::GetInstance()->m_RightLimit + 5) 			
					{
						PostFound = 2;
					}
				}
				else
				{
					if( pan < Head::GetInstance()->m_RightLimit + 5) 			
					{
						PostFound = 2;
					}

					else if(GoalPercept::GetInstance()->Status == GoalPercept::POSSIBLE_RIGHT_POST
					|| GoalPercept::GetInstance()->Status == GoalPercept::POSSIBLE_LEFT_POST
					|| GoalPercept::GetInstance()->Status == GoalPercept::LEFT_POST
					|| GoalPercept::GetInstance()->Status == GoalPercept::RIGHT_POST)
					{
						if(ObjectPos.X > (Camera::WIDTH/2.0 - 30) && ObjectPos.X < (Camera::WIDTH/2.0 + 30))
						{
								PostFound = 2;
								PanPostLeft = PanPostLeft;
								PanPostRight = pan - (ObjectPos.X - Camera::WIDTH/2.0) * (Camera::VIEW_H_ANGLE/Camera::WIDTH);
								PanPostBoth = 0;
								Head::GetInstance()->initSearchingPost(RIGHT);
						}
						else if(pan < Head::GetInstance()->m_RightLimit + 5 && ObjectPos.X >= (Camera::WIDTH/2.0 + 30))//tambahan 210414
						{
								PostFound = 2;
								PanPostLeft = PanPostLeft;
								PanPostRight = -90 - (ObjectPos.X - Camera::WIDTH/2.0) * (Camera::VIEW_H_ANGLE/Camera::WIDTH);
								PanPostBoth = 0;
								Head::GetInstance()->initSearchingPost(RIGHT);
						}
						Tracker.ProcessCheckPost(ObjectPos);
						Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
					}
					else if(GoalPercept::GetInstance()->Status == GoalPercept::BOTH_POST)
					{
						if(ObjectPos.X > (Camera::WIDTH/2.0 - 30) && ObjectPos.X < (Camera::WIDTH/2.0 + 30))
						{
								PostFound = 2;
								PanPostLeft = 0;
								PanPostRight = 0;
								PanPostBoth = pan;
						}
						Tracker.ProcessCheckPost(ObjectPos);
						Follower.ProcessLocalizeGoalPost(Tracker.ball_position);
					}
				}
				printf("Post Found 1\n");
				printf("\nPanPostLeft = %lf \n", PanPostLeft);
				printf("\nPanPostRight = %lf \n", PanPostRight);
				printf("\nPanPostBoth = %lf \n", PanPostBoth);
			}
			else if(PostFound == 2)	
			{
				/*
				printf("error = %lf \n", CompassError);
				printf("\nPanPostLeft = %lf \n", PanPostLeft);
				printf("\nPanPostRight = %lf \n", PanPostRight);
				printf("\nPanPostBoth = %lf \n", PanPostBoth);*/

				if(abs(CompassError) <= 110)//MELIHAT KE GAWANG LAWAN
				{
					if(PanPostBoth == 0)
					{
						if(PanPostLeft != 0 && PanPostRight == 0)//kalo cuma melihat satu tiang, ngecek compassnya juga
						{
							if((PanPostLeft < 0 && CompassError <= 0) || (PanPostLeft > 0 && CompassError >= 0))
							{
								PanPostBoth = -CompassError + PanPostLeft*1/2;
								//PanPostBoth =  PanPostLeft*1.5;
								PostPosition = -2;//liat satu gawang
							}
							else
							{
								PanPostBoth = -CompassError;
								PostPosition = -1;//
							}
						}
						/*else if(PanPostLeft > 15 && PanPostRight < -15)//robot berada diantara kedua tiang, langsung sikat...
						{
							PanPostBoth = 0;
							PostPosition = 1;//diantara 2 gawang
						}*/
						else if(PanPostLeft != 0 && PanPostRight != 0)//kalo dapet 2 tiang, cek rata2nya
						{
							PanPostBoth = (PanPostLeft + PanPostRight)/2;//DIGANTI, TADINYA DIBAGI 2
							PostPosition = 2;//dapet 2 gawang tapi ga di tengah
						}
						else if(PanPostLeft == 0 && PanPostRight == 0)//error robotnya kurang dari 120, tapi ga nemu gawang
						{
							PanPostBoth = -(CompassError);// + (CompassError * 45 /(abs(CompassError)));
							PostPosition = 0;//ga nemu gawang
						}
					}
					else
					{
						PanPostBoth = PanPostBoth;
						PanPostLeft = 0;
						PanPostRight = 0;
						PostPosition = 1;
					}

					if((PanPostBoth - CompassError) > 110)
						PanPostBoth = CompassError;
				}
				else//MELIHAT KE GAWANG SENDIRI
				{
					PostPosition = 0;
					if((int) PanPostBoth == 0)
					{
						PanPostBoth = (PanPostLeft+PanPostRight)/2;
					}
					if(CompassError > 0)//kanan set point
					{
						if(PanPostBoth > 0)
							PanPostBoth = -CompassError - 90;
						else
							PanPostBoth = CompassError; 			
					}
					else//kiri set point
					{
						if(PanPostBoth < 0)
							PanPostBoth = CompassError + 90;
						else
							PanPostBoth = CompassError;
					}
					if(abs(PanPostBoth) > 170)
						PanPostBoth = 170;
				}
				//printf("\n\nPanPostBoth = %lf \n", PanPostBoth);
				//RETURN ALL PARAMETER TO DEFAULT	
				WaitUntilReady = true;
        			CountToReady = 0;
       				PostFound = 0;

				//limit head diubah ke default lagi.
				Head::GetInstance()->m_LeftLimit = 70;
				Head::GetInstance()->m_RightLimit = -70;
       			set_aproaching_ball();
				return;
			}
		}
		else
		{
			CountToReady++;
		}
	}
}


void Behavior::ProcessAproachingBall2()
{
	//printf("PROCESS APROACHING BALL 2 ----------------------------------\n");
	double tilt = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);

	if(WaitUntilReady)//BELUM READY UNTUK TURNING BALL
	{
		Walking::GetInstance()->X_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->Y_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->A_MOVE_AMPLITUDE = 0;

		X_Moved = Walking::GetInstance()->Get_X_Moved();
		X_Moved = 0;
		Head::GetInstance()->MoveByAngle(PanLastSeenBall, (Head::GetInstance()->GetBottomLimitAngle()));
		mark = 0;

    		if(CountToReady > MaxCountToReady + 4)//counter untuk nunggu sampe kepalanya turun
    		{
      			WaitUntilReady = false;
      			CountToReady = 0;
    		}
    		else
    		{
    			CountToReady++;
	  	}

		/*sp = ObjectSize;//ini diisi set point untuk muter pertama kali, nilainya masih get angle8bit
		if (sp > 128)
    		{
    			kompensator = sp - 128;
    		}
    		else
    		{
    			kompensator = 128 - sp;
    		}*/
  	}
	else
	{
	    	cmps_now = -ObjectSize * 360/255;
    		/*if (sp > 128)
    		{
    			if (cmps_now >= kompensator)
    			  	cmps_error = cmps_now - kompensator - 128;
      			else 
        			cmps_error = 127 + cmps_now - kompensator;
    		}
    		else
    		{
    			if (cmps_now >= 255 - kompensator)
        			cmps_error = cmps_now + kompensator - 383;
      			else
        			cmps_error = cmps_now + kompensator -128;
    		}*/

		if(tilt < Head::GetInstance()->GetTopLimitAngle() - 20 && mark == 0)//kepalanya naik
		//kalo udah dapet maka akan nengahin bola, jadi intinya sama kayak tracker.process
		{
			Tracker.ProcessUpHead(ObjectPos);
			countHeadSearchingCycle = 0;
		}
		else if(tilt > Head::GetInstance()->GetTopLimitAngle() - 20 && mark == 0)//kalo kepalanya udah naik
		{
			mark = 1;
		}
		else
		{
			Tracker.Process(ObjectPos);
		}

		Follower.ProcessAproachingBall2(Tracker.ball_position, PanPostBoth, cmps_now, CompassPost);

		X_Moved += Walking::GetInstance()->Get_X_Moved();
		//printf("X Move =  %lf \n", X_Moved);
		if(State == DRIBBLE_STATE)
		{
//			if(PanPostBoth != -1)			
//				PanPostBoth =  PanPostBoth * (1 + abs(CompassError) * X_Moved / 800);
			//printf("\nPanPostBoth = %lf \n", PanPostBoth);
			//printf("\nX_Moved = %lf \n", X_Moved);
			WaitUntilReady = true;
			CountToReady = 0;
			X_Moved = 0;
			mark =0;
		}
	}
}

void Behavior::turningBall()
{
	//printf("TURNING BALL------------------------\n");
	
	startKickOff = false;
  double pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
  double tilt = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);
  double tilt_min = Head::GetInstance()->GetBottomLimitAngle();
  double tilt_range = Head::GetInstance()->GetTopLimitAngle() - tilt_min;
  double tilt_percent = (tilt - tilt_min) / tilt_range;
  if(tilt_percent < 0)
    tilt_percent = -tilt_percent;

	if(WaitUntilReady)
	{
		if(CountToReady > MaxCountToReady)
		{
	 	  PanPostBoth = -PanPostBoth * 0.75;
			WaitUntilReady = false;
			CountToReady = 0;
		}
		else
		{
  		CountToReady++;
		}
	}
	else
	{
    
   		 cmps_now = ObjectSize;
   		 if (sp > 128)
   		 {
    			if (cmps_now >= kompensator)
			{
      				cmps_error = cmps_now - kompensator - 128;
      			}
			else
			{ 
        			cmps_error = 127 + cmps_now - kompensator;
    			}
		}
    		else
    		{
    			if (cmps_now >= 255 - kompensator)
        			cmps_error = cmps_now + kompensator - 383;
      			else
        			cmps_error = cmps_now + kompensator -128;
		}

		//printf("Compass = %d \n", cmps_error);
  		Tracker.Process(ObjectPos);
		Follower.ProcessTurnAroundBall(Tracker.ball_position, PanPostBoth, cmps_error, PostPosition);
    		if(State == NORMAL_STATE || State == DRIBBLE_STATE || State == CHECK_POST_STATE || State == KICK_STATE)
    		{
	    		Walking::GetInstance()->stop_counting_step();
      			CountToReady = 0;
  			WaitUntilReady = true;
 		}
  	}
}

void Behavior::dribbleBall()
{
	startKickOff = false;
	if(WaitUntilReady)
	{
		if(CountToReady > MaxCountToReady)
		{
			WaitUntilReady = false;
			CountToReady = 0;
		}
		else
		{
			CountToReady++;
		}
	}
/*
	else
	{
		sp = ObjectSize;//ini diisi set point untuk muter pertama kali, nilainya masih get angle8bit
		if (sp > 128)
    		{
    			kompensator = sp - 128;
    		}
    		else
    		{
    			kompensator = 128 - sp;
    		}
  	}
*/
	else
	{
	    	cmps_now = -ObjectSize * 360/255;
    		/*if (sp > 128)
    		{
    			if (cmps_now >= kompensator)
			{
    		  		cmps_error = cmps_now - kompensator - 128;
      			}
			else
			{ 
        			cmps_error = 127 + cmps_now - kompensator;
			}
    		}
    		else
    		{
    			if (cmps_now >= 255 - kompensator)
			{
        			cmps_error = cmps_now + kompensator - 383;
      			}
			else
			{
        			cmps_error = cmps_now + kompensator -128;
    			}
		}*/
		Tracker.ProcessToKick(ObjectPos);
		Follower.ProcessToKick(Tracker.ball_position, PanPostBoth, cmps_now, CompassPost);
	}
}

void Behavior::PenaltyKick()
{
	//printf("PENALTY KICK>>>>>>>>>>>>>>>>>>>>>>>>>\n");

//	Tracker.ProcessKick(ObjectPos);
	Tracker.Process(ObjectPos);//DIUBAH
        Follower.ProcessPenaltyKick(Tracker.ball_position);

	if(Action::GetInstance()->IsRunning() == 0)
        {
                Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
                Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);

//		if(Follower.KickBall == 0)
//		{
			Walking::GetInstance()->Start();
//		}
                if(Follower.KickBall != 0)
                {
           	    Walking::GetInstance()->Stop();
	            Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
                    Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
		    //printf("KickBall = %d \n", Follower.KickBall);
			usleep(8000);
                    if(Follower.KickBall == -1)
                    {
			usleep(8000);
                        Action::GetInstance()->Start(12);//123   // RIGHT KICK
                        fprintf(stderr, "RightKick! \n");
                    }
                    else if(Follower.KickBall == 1)
                    {
			usleep(8000);
                        Action::GetInstance()->Start(13);//117   // LEFT KICK
                        fprintf(stderr, "LeftKick! \n");
                    }
                }
        }
	else
	{
		Walking::GetInstance()->Stop();
	}
}
