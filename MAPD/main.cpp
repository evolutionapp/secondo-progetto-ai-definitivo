//
// Created by Zhe Chen on 10/6/20.
//
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include "Agent.h"
#include "agents_loader.h"
#include "Task.h"
#include "TaskAssignment.h"
#include "ICBSSearch.h"
#include "CBSHOnline.h"
#include "PathPlanning.h"
#include "map_loader_with_cost.h"
#include "TaskAssignmentPP.h"
#include "TaskAssignmentRegret.h"
#include "TaskAssignmentTaskHeap.h"
#include "TaskAssignmentTaskHeapRegret.h"
#include "TaskAssignmentRegretTask.h"
#include "TaskAssignmentPPTask.h"
#include "online_simu.h"
int map_cols=0;
int screen = 1;
/*
./MAPD 
-m kiva-20-500-5.map 
-a kiva-20-500-5.map 
-t 0.task 
--capacity 1 
-o output_PP_2.txt 

./MAPD -m kiva-20-500-5.map -a kiva-20-500-5.map -t 0.task --capacity 1 -o output_PP_2.txt 
*/
int main(int argc, char** argv){
    namespace po = boost::program_options;

    //Leggo l'input da comando
    po::options_description desc("Allowed options");
    desc.add_options()
        ("map,m", po::value<std::string>()->required(), "input file for map")
        ("agents,a", po::value<std::string>()->required(), "initial locations for agents")
        ("tasks,t", po::value<std::string>()->required(), "a set of tasks")
        ("output,o", po::value<std::string>()->required(), "output file for schedule")
        ("assignment,A", po::value<std::string>()->required(), "assignment file")
        ("capacity", po::value<int>()->default_value(1), "cutoff time (seconds)");

    po::variables_map temp;
    po::store(po::parse_command_line(argc, argv, desc), temp);
    const po::variables_map vm = temp;    
    string task = vm["tasks"].as<string>();
    string agent = vm["agents"].as<string>();
    string map = vm["map"].as<string>();
    string output;
    if (vm.count("output")){
        output = vm["output"].as<string>();
    }
    else
        output = "";

    string assignmentFile;
    if (vm.count("assignment")){
        assignmentFile = vm["assignment"].as<string>();
    }
    else
        assignmentFile = "";

    int capacity = vm["capacity"].as<int>();

    
    bool multi_label = false;
    bool real_cost = false;  
    bool regret = false;  //true per RMCA false per MCA
    bool regret_task = false;
    srand(0);


    //Setto le opzioni 
    options options1;
    options1.multi_label = multi_label;
    options1.window_size = 0;
    options1.RM4way = 1;
    options1.f_w = 1;
    options1.ignore_target = true;
    
    constraint_strategy s = constraint_strategy::PP;
    options1.s = s;



    //Caricamento dati Kiva 
    MapLoaderCost* mapLoader;
    AgentLoader* agentLoader;
    TaskLoader* taskLoader;

    mapLoader = new MapLoaderCost();
    agentLoader = new AgentLoader();
    taskLoader = new TaskLoader();

    
    mapLoader->loadKiva(map);
    agentLoader->loadKiva(agent,capacity,*mapLoader);
    taskLoader->loadKiva(task,*mapLoader);
    

    cout<<"- Mappa caricata"<<endl;
    
    map_cols = mapLoader->cols;

    cout<<"- Task e agenti caricati"<<endl<<endl;

    cout<<"AGENTI :"<<endl;
    agentLoader->print();

    cout<<endl<<"TASKS :"<<endl;
    taskLoader->printTasks();
    cout<<endl;
    //Imposto l'obiettivo
    TA_Options ta_options;
    ta_options.objective = OBJECTIVE::TOTAL_TRAVEL_DELAY;  
    ta_options.max_iteration =10000;
    ta_options.only_top = true;  //Aggiorna solo il top dello Heap

    TaskAssignment* taskAssignment;
    if (regret_task){
        cout<<"ALGORITMO USATO : PP Regret Task"<<endl<<endl;
        taskAssignment = new TaskAssignmentRegretTask(agentLoader,taskLoader,mapLoader, ta_options, options1,real_cost,0 /*time_limit*/);
    }
    else if (!regret){
        cout<<"ALGORITMO USATO : PP"<<endl<<endl;

        taskAssignment = new TaskAssignmentPP(agentLoader,taskLoader,mapLoader, ta_options, options1,real_cost,0 /*time_limit*/);
    }


    // Inserisce tutti i task nella lista di task non assegnati
    taskAssignment->initializeOneShot();

    //Assegnamento task (RMCA)
    taskAssignment->assignTasks(assignmentFile);
    
    


    int ideal_cost = 0;
    int ideal_delay = 0;
    int total_cost = 0;
    int total_delay = 0;
    for (Assignment& assign: taskAssignment->assignments){
        ideal_cost += assign.ideal_cost;
        ideal_delay += assign.actions.back().current_total_delay;
        if (assign.path.size()!= 0)
            total_cost += assign.path.size()-1;
        total_delay +=assign.current_total_delay;
    }
    if (screen>=1){
        ofstream stats;
        stats.open(output, ios::app);
        stats<<"AZIONI AGENTI : "<<endl;
        stats.close();
        taskAssignment->printAssignments(output);


         stats;
        stats.open(output, ios::app);
        stats<<endl<<endl<<"PERCORSI AGENTI : "<<endl;
        stats.close();
        taskAssignment->printPath(output);
    }

    if(options1.s == constraint_strategy::PP || vm.count("only-assignment")){

        cout <<"Number agents: "<<agentLoader->num_of_agents<<" Number tasks: "<<taskLoader->num_of_tasks
                << " Ideal total cost: " << ideal_cost
                <<", Real cost: "<< total_cost
                <<", Makespan: "<< taskAssignment->current_makespan
                <<", Ideal total delay: "<< ideal_delay
                <<", Real dealy: "<< total_delay<<", TA Runtime: "<<taskAssignment->runtime/CLOCKS_PER_SEC<<
                ", No of agents with tasks: "<< taskAssignment->get_num_agents_with_tasks()<<endl;
        cout << "runtime_pp: "<< taskAssignment->runtime_pp/CLOCKS_PER_SEC<<
                " runtime_update_conflict: "<< taskAssignment->runtime_update_conflict/CLOCKS_PER_SEC<<
                " runtime_update_changed_agent: "<<taskAssignment->runtime_update_changed_agent/CLOCKS_PER_SEC<<
                " num_conflict_updates: "<< taskAssignment->num_conflict_updates<<
                " num_task_assign_updates: "<< taskAssignment->num_task_assign_updates<<
                endl;

        // if (output!="") {

        //     ofstream stats;
        //     stats.open(output, ios::app);
        //     stats <<"1"<<","<<agentLoader->num_of_agents<<
        //             ","<<taskLoader->num_of_tasks<<
        //             ","<< ideal_cost <<","<<total_cost << "," << ideal_delay <<","<< total_delay << ","
        //             <<taskAssignment->current_makespan<<","<<taskAssignment->current_makespan<<","
        //             << taskAssignment->get_num_agents_with_tasks()<< "," << taskAssignment->runtime / CLOCKS_PER_SEC
        //             << "," << taskAssignment->runtime_pp / CLOCKS_PER_SEC
        //             << "," << taskAssignment->runtime_update_conflict / CLOCKS_PER_SEC <<
        //             "," << taskAssignment->runtime_update_changed_agent / CLOCKS_PER_SEC<<
        //             ","<< taskAssignment->num_of_pp <<
        //             ","<< taskAssignment->num_conflict_updates<<
        //             ","<< taskAssignment->num_task_assign_updates<<
        //             "," <<endl;
        //     stats.close();
        // }

    }
    else{
//        if( options1.s == constraint_strategy::PP){
//            options1.s = constraint_strategy::CBS;
//            options1.f_w = 1.1;
//        }
        PathPlanner* planner;
        bool success;

        planner = new PathPlanner(taskAssignment->assignments,0 /*time_limit*/,s,screen, options1,mapLoader);
        success = planner->startPlan();

        if(screen >=1)
            planner->printPath();
        if (success){
            cout<<"Success; ";
        }
        else{
            cout<<"Failed; ";
        }
        int final_cost=0;
        int makespan = 0;
        for(vector<PathEntry>& p : planner->plans){
            final_cost+=p.size()-1;
        }
        for (auto& assign : taskAssignment->assignments){
            if (assign.actions[assign.actions.size()-2].real_action_time > makespan)
                makespan = assign.actions[assign.actions.size()-2].real_action_time;
        }
        int final_delay = 0;
        for (auto& info : planner->task_info_table){
            final_delay+=info.second.delay;
        }
        cout<<"Ideal total delay: "<<ideal_delay  <<", Final delay: "<< final_delay<<", Ideal cost: "<<ideal_cost << ", Final_cost: " << final_cost << ", Cost increase: "
            <<", ideal Makespan: "<< taskAssignment->current_makespan << ", Makespan"<<makespan<<
            ", TA Runtime: "<<taskAssignment->runtime/CLOCKS_PER_SEC<<", Path Planning Runtime: "<<planner->runtime<<
            ", No of agents with tasks: "<< taskAssignment->get_num_agents_with_tasks()<<endl;

        // if (output!="") {
        //     ofstream stats;
        //     stats.open(output, ios::app);
        //     stats <<success<<","<<agentLoader->num_of_agents<<
        //             ","<<taskLoader->num_of_tasks<<
        //             "," << ideal_cost << "," << final_cost << "," << ideal_delay << "," << final_delay << ","
        //             <<taskAssignment->current_makespan<<","<<makespan<<","
        //             << taskAssignment->runtime / CLOCKS_PER_SEC << "," << planner->runtime << ","
        //             << taskAssignment->get_num_agents_with_tasks() << "," <<planner->num_replan<<","<< endl;
        //     stats.close();
        // }

    }

    








}
