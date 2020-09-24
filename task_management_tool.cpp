
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

#include "task_management_tool.h"

using namespace std; 

void WorkPlan::display(bool verbose,bool testing)
{
	string inone="***";
	if (head!=NULL)
	{
		Task *pivot =new Task;
		Task *compeer =new Task;
			
		pivot=head;
		do
		{
			if(testing)
				inone+=" ";
			else
				cout << pivot->day <<". DAY"<< endl;
			compeer=pivot;
			while(compeer!=NULL)
			{
				string PREV= compeer->previous!=NULL?compeer->previous->name:"NULL";
				string NEXT=compeer->next!=NULL?compeer->next->name:"NULL";
				string CONT=compeer->counterpart!=NULL?compeer->counterpart->name:"NULL";
				if (testing)
					inone+=compeer->name;
				else
					if(verbose)
						cout<<"\t"<<setw(2)<< compeer->time <<":00\t"<< PREV<<"\t<- "<< compeer->name <<"("<< compeer->priority <<")->\t"<<NEXT <<"\t |_"<<CONT<<endl;
					else
						cout<<"\t"<<setw(2)<< compeer->time <<":00\t"<< compeer->name <<"("<< compeer->priority <<")"<<endl;				
				compeer=compeer->counterpart;
			}
			pivot=pivot->next;
		}while(pivot!=head);
		if(testing)
		{
			cout<<inone<<endl;
			cout<<"(checking cycled list:";
			if (checkCycledList())
				cout<<" PASS)"<<endl;
			else
				cout<<" FAIL)"<<endl;
		}
	}
	else
		cout<<"There is no task yet!"<<endl;
}

int WorkPlan::getUsableDay()
{
	return usable_day;
}

int WorkPlan::getUsableTime()
{
	return usable_time;
}


void WorkPlan::create()
{		
	head = NULL;
}

void WorkPlan::close()
{
   Task *traverse=head;
    Task *tail;
    while(head!=NULL){
    	traverse=head; //silinecek g�n� tut
    	(head->next)->previous=head->previous;
    	(head->previous)->next=head->next; // ba�lant�lar� kapat�yoruz
    	head->previous=NULL;
      	head=head->next;
    	tail=traverse->counterpart;
     	while(tail!=NULL){
      	 	traverse->counterpart=traverse->counterpart->counterpart;
       		delete tail;
       		tail=traverse->counterpart;
        }
        if(head->next==head && head->counterpart==NULL){ //head->next i null lamam�st�k. null olunca  sona geldik demektir
          	delete traverse;
          	break;
        }
        delete traverse;
    }
}

void WorkPlan::add(Task *task)
{
	Task *newtask;
	newtask= new Task;
	newtask->name=new char [strlen(task->name)];
	strcpy(newtask->name, task->name);
	newtask->day = task->day;
	newtask->time = task->time;
	newtask->priority = task->priority;
	newtask->previous = NULL;
	newtask->next = NULL;
	newtask->counterpart = NULL;
	if(head == NULL){ //if linked list is empty
		head = newtask;
		head->next=head;
		head->previous=head;
		return;
	}
	if(newtask->day  <  head->day){ //add to day before head
		newtask->next=head;
		newtask->previous = head->previous;
		(head->previous)->next=newtask;
		head->previous = newtask;
		head = newtask;
		return;  //head changes, old head is newtask->next now
	}
	Task *tail;
	Task*traverse = head;
    while(newtask->day  > traverse->day && traverse->next != head){
        tail=traverse;  // tail traverse �n 1 ad�m gerisinden gelmeli. listeye eklemek i�in tail->next kullanaca��z
        traverse=traverse->next;
    }
	if(newtask->day == traverse->day){ //add new task to a pre-existing day   DAHA �NCE VAR OLAN G�NE TASK EKLEME HEAD YA DA DAHA SONRAKI GUNLERDEN BIRI
		if(newtask->time < traverse->time){
			newtask->counterpart=traverse;
			(traverse->next)->previous=newtask;
			(traverse->previous)->next=newtask;
			newtask->next = traverse->next;
			newtask->previous = traverse->previous;
			traverse->next=NULL;  
			traverse->previous=NULL;
			if(traverse == head){ //bulundugumuz gun head in gunu de olabilir bu durumda head degisiyor
				head=newtask;
			}
			return;   // one of main tasks changes . new main task is newtask, traverse(old main task) is its counterpart now
		}
		else{ //ayn� g�ne ekleme yapmaya devam ediyoruz. ama yeni task �n saati daha b�y�k oldu�u i�in g�n�n i�ine girip yeteri kadar a�a�� iniyoruz
			tail=traverse;
			while(newtask->time >= traverse->time &&  tail->counterpart != NULL){
				tail = traverse;
				traverse = traverse->counterpart;
			}
			if(newtask->time != tail->time && tail->counterpart != NULL){ //  bir g�n�n iki farkl� randevusunun aras�na yeni randevu
				tail->counterpart = newtask;
				newtask->counterpart = traverse;
				return;
			}
			if(newtask->time == tail->time){
				if(newtask->priority > tail->priority){//TAIL DELAYING...
					int temp_priority = tail->priority;
					char temp_name[3];
					strcpy(temp_name,tail->name); // tailin bilgilerini  temp_priority temp_name gibi de�i�kenlerle yedekledik
					tail->priority=newtask->priority;
					strcpy(tail->name,newtask->name);//newtask ile tailin bilgilerini birbiri ile de�i�tirdik 
					strcpy(newtask->name,temp_name);
					newtask->priority=temp_priority;
					checkAvailableNextTimesFor(newtask);
					newtask->day=getUsableDay();
					newtask->time=getUsableTime();
					add(newtask);// add func can be a racusive function
					return;	
				}
				if(newtask->priority < tail->priority){  //NEWTASK DELAYING...
					checkAvailableNextTimesFor(tail); //iceri taili att�m cunku checkUsable... fonk  listede var olan bir taska gore calisiyor.  tailin ve newtask �n gun ve saati esit old icin bu durum problem cikartmayacak
					task->day=getUsableDay();
					task->time=getUsableTime();
					add(task);
					return;			
				}
			}
			if(tail->counterpart ==NULL){
				tail->counterpart = newtask;
				return;  // add end of the day;
			}			
		}
	}
	if(newtask->day  <  traverse->day){ // add new task between two days
		tail->next = newtask; //tail ile traverse in aras�na ekle
		newtask->next = traverse;
		traverse->previous = newtask;
		newtask->previous = tail;
		return;
	} 
	if(newtask->day > traverse->day){// add last day
		traverse->next = newtask;
		head->previous=newtask;
		newtask->next=head;
		newtask->previous = traverse;
	}
}

Task * WorkPlan::getTask(int day, int time)
{
	Task *traverse = head;
	while(traverse->day != day){
		traverse = traverse->next;  // finds given day
	}// istenen g�n� buldu
	while(traverse->time != time){
		traverse = traverse->counterpart; // finds given time
	}//istenen saati buldu
	return traverse;
}

void WorkPlan::checkAvailableNextTimesFor(Task *delayed)
{
	Task *traverse,*tail;
	traverse = head->next;
	tail = head;
	bool found = false;
	while(tail->day != delayed->day){
		tail=tail->next;
		traverse=traverse->next;
	}
	while(tail->time != delayed->time){
		tail=tail->counterpart;
	} // tail finds delayed
	while(!found){
		while(tail->counterpart!=NULL  && (tail->counterpart)->time - tail->time == 1){
			tail = tail->counterpart;
		}
		if(tail->counterpart != NULL){
			usable_day=tail->day;
			usable_time=tail->time +1;
			return;
		}
		if(tail->counterpart == NULL){//g�n i�erisinde available time yoksa
			tail=traverse;
			traverse=traverse->next;
		}
		if(tail->time>8){
			usable_day=tail->day;
			usable_time=8;
			return;
		}
	}
}

void WorkPlan::delayAllTasksOfDay(int day)
{
	Task *traverse,*tail;
	traverse=head;
	tail=head;
	while(tail->day != day){ // head in g�n�n� bo�alt�yosak buraya hi� girmeyecek
		tail=traverse;
		traverse = traverse->next; //tail traverse in bir g�n gerisinden geliyor
	} //tail ertelemek istedi�imiz g�n�n tamam�n� tutuyor(counterpartlarla beraber)
	if(tail->day == head->day){
		traverse=traverse->next;
		head=traverse;
	}
	(tail->next)->previous=tail->previous;
	(tail->previous)->next=traverse;
	tail->next=NULL;
	tail->previous=NULL;
	Task *tail2;
	while(tail != NULL){
		tail2=tail;
		if(traverse->time>8){   // g�n�n ilk saati bo�sa ilk saati doldur
			tail->day=traverse->day;
			tail->time=8;
			add(tail);
			tail=tail->counterpart;
			delete tail2;
		}
		else{ // bir g�n sonras�n�n ilk saati i�in uygun saat ar�yorum, ��nk� checkAvailable ye listede olan bir task � atmal�y�m. taili listeden cikarmistim daha once
			checkAvailableNextTimesFor(traverse);
			tail->time=getUsableTime();
			tail->day=getUsableDay();
			add(tail); //uygun saat ve g�n� bulunca listeye yeniden ekledik
			tail=tail->counterpart;
			delete tail2;
		}
	}
}

void WorkPlan::remove(Task *target)
{
	Task *iter, *tail;
	iter=head;
	if(target == head){
		if(head->counterpart ==NULL){  //eger head degisiyorsa
			head=target->next;
			head->previous=target->previous;
			target->previous->next=head;
			delete target;
			return;
		}
		else{
			head = target->counterpart;
			target->counterpart->next = target->next;
			target->counterpart->previous = target->previous;
			target->previous->next = target->counterpart;
			target->next->previous = target->counterpart;
			delete target;
			return;
		}
	}
	while(target->day != iter->day){
		iter = iter->next;
	} // silinecek task�n bulundu�u g�n� bulduk
	if(iter->counterpart==NULL){
		iter->next->previous=iter->previous;
		iter->previous->next = iter->next;
		iter->next = NULL;
		iter->previous=NULL;
		delete iter;
		return;
	}
	else{
		if(target->time == iter->time){ //  if target is a main node different from head
			iter->counterpart->next=iter->next;
			iter->counterpart->previous = iter->previous;
			iter->previous->next = iter->counterpart;
			iter->next->previous = iter->counterpart;
			delete iter;
			return;
		}
		if(target->time != iter->time){
			tail = iter;
			iter=iter->counterpart;
			while(target->time != iter->time){
				iter=iter->counterpart;
				tail=tail->counterpart;
			}
			tail->counterpart = iter->counterpart;
			delete iter;
			return;
		}
	}
}

bool WorkPlan::checkCycledList()
{				
	Task *pivot=new Task();
	pivot=head;
	int patient=100;
	bool r=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->previous;
		if(pivot==head)
		{
			r=true;
			break;
		}
	}
	cout<<"("<<100-patient<<")";
	patient=100;
	bool l=false;
	while (pivot!=NULL&&patient>0)
	{		
		patient--;
		pivot=pivot->next;
		if(pivot==head)
		{
			l=true;
			break;
		}
	}
	return r&l;
}
