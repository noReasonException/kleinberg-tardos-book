// same number of men and women
//each row, the nth mans prefered list
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>
typedef int man;
typedef int woman;
void debug2D(std::vector<std::vector<int> >&data){
	for(auto i:data){
		for(auto j:i){
			std::cout<<j<<"\t";
		}
		std::cout<<std::endl;
	}
}
void debug(std::vector<std::tuple<man,woman> >&data){
	for(auto i:data){
		std::cout<<"("<<std::get<0>(i)<<", "<<std::get<1>(i)<<")"<<std::endl;
	}
}
void debug(std::vector<int> &data,std::string reason){
	std::cout<<"Debug on "<<reason<<std::endl;
	for(auto i:data){
		std::cout<<i<<" ,";
	}
	std::cout<<std::endl;
	std::cout<<"Debug on "<<reason<<" Terminates Here"<<std::endl;
}
std::vector<int> parseLine(const std::string &line){
	//Will only work for n<10 ffs;
	std::vector<int> result(0);
		for(char i:line){
			if(i==' '||i=='\n') continue;
			result.push_back(i-'0');
		}
		return result;
}

std::vector<std::vector<int> > getInput(const std::string &filename){
	std::ifstream input_stream(filename);
	std::vector<std::vector<int> > result;
	if(!input_stream) {
		std::cout<<"Unable to find file "<<filename;
		return result;
	}
	std::string eachLine;
	while(getline(input_stream,eachLine)){
		result.push_back(parseLine(eachLine));
	}
	if(result.size()%2!=0) throw std::invalid_argument("Expected input lines to be divisible by 2");
	return result;

}

/*
 * For every man
 * 	1.select the maximum woman he can pair 
 *  2.pair
 *  
 * For every woman
 *  1. if you receive a request to pair
 * 		if you are free -> pair immediately
 * 		if you are engaged but the man requesting you is higher on your preference list -> pair
 * 		else reject request
 * 
*/
std::array<std::vector<std::vector<int> >,2> splitPreferences(std::vector<std::vector<int> > &preferences){
	std::vector<std::vector<int> > men_preferences;
	std::vector<std::vector<int> > women_preferences;
	for(int i=0;i<preferences.size()/2;i++)men_preferences.push_back(preferences[i]);
	for(int i=preferences.size()/2;i<preferences.size();i++)women_preferences.push_back(preferences[i]);
	std::array<std::vector<std::vector<int> >,2> split {men_preferences,women_preferences};
	return split;
}

std::vector<std::tuple<man,woman> > generateCouplesVector(const std::vector<int> &womenStatus){
	std::vector<std::tuple<man,woman> > result;
	for(int everyWomanId=0;everyWomanId<womenStatus.size();everyWomanId++){
		result.push_back(std::make_tuple(womenStatus[everyWomanId],everyWomanId));
	}
	return result;
}

std::vector<std::tuple<man,woman> > stableMatch(std::vector<std::vector<int> > &preferences){
	size_t n = preferences.size()/2;
	std::vector<int> womanStatus(n,-1); //will contain -1 for free woman, or k the current husband id
	std::vector<int> freeMen(n); //will contan all the free men, the algorithm will terminate when this vector goes empty

	//initially every man is free, and every woman is also free (-1,-1,-1,....,-1)
	int id = n;
    std::generate(freeMen.begin(), freeMen.end(), [&id]() { return --id; });

    //convinient 
	std::vector<std::tuple<man,woman> >couples;
	auto [men_preferences,women_preferences] = splitPreferences(preferences);
	
	int currentWoman = 0;
	int currentMen = 0;

	int currentHusband=0;
	int currentHusbandPriority=0;
	int currentMenPriority=0;

	while(!freeMen.empty()){
		currentMen = freeMen.back(); //get any free men
		freeMen.pop_back(); //remove him from free mens queue


		std::cout<<"For men with id"<< currentMen <<std::endl;
		//for every of its preferences
		for(int woman_preference_index=0;woman_preference_index<men_preferences[currentMen].size();woman_preference_index++){


			currentWoman = men_preferences[currentMen][woman_preference_index];
			std::cout<<"\tFor every preffered women (preference_index "<< woman_preference_index <<") with name "<<currentWoman<<" - engaged? - "<<std::boolalpha<<(womanStatus[currentWoman] != -1) << std::endl;
 
			if((womanStatus[currentWoman]) == -1){
				//woman is free, immediately say yes and register the current husbands priority to the isWomanEngaged vector
				womanStatus[currentWoman]=currentMen;
				std::cout<<"\t\tEngaged!"<<std::endl;
				break;
			}
			//if woman is engaged, then the current husbands priority(tuple._2) should be less than the proposed one(men_id)
			currentHusband = womanStatus[currentWoman];
			currentHusbandPriority = std::find(women_preferences[currentWoman].begin(),women_preferences[currentWoman].end(),currentHusband)- women_preferences[currentWoman].begin();
			currentMenPriority = std::find(women_preferences[currentWoman].begin(),women_preferences[currentWoman].end(),currentMen)- women_preferences[currentWoman].begin();
			std::cout<<"\t\tcurrentHusband "<<currentHusband<<" with priority "<<currentHusbandPriority<<std::endl;
			std::cout<<"\t\tcurrentMen "<<currentMen<<" with priority "<<currentMenPriority<<std::endl;
			if(currentMenPriority<currentHusbandPriority){
				freeMen.push_back(currentHusband);
				womanStatus[currentWoman] = currentMen;
				//break from current husband, engage current one
				// std::remove_if(couples.begin(),couples.end(),lookFor);
				//couples.push_back(std::make_tuple('0'+men_id,everyWoman));
				std::cout<<"\t\t\tBreak Condition Detected"<<std::endl;
				break;
			}
			//else ignore request
		}
	}
	// debug(isWomanEngaged,"isWomanEngaged");
	return generateCouplesVector(womanStatus);
}


int main(int argc, char* argv[]){
	std::vector<std::vector<int > > data = getInput(argv[1]);
	auto result = stableMatch(data);
	std::cout<<"Finished"<<std::endl;
	debug(result);

}