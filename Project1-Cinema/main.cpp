#include<iostream>
#include<vector>
#include <sstream>
#include <string>
#include<fstream>
#include<map>
#include <algorithm>
using namespace std;

struct Movie
{
	string CinemaName;
	string MovieName;
	string Day;
	string StartingTime;
	string FinishingTime;
	string Price;
	string Hall;
};

typedef vector<Movie* > MovieList;

//This function extract hour and minute from HH:MM format
void extractTime(string time,int &hour,int &minute)
{
	stringstream ss(time);
	int index = 1;
	while( ss.good() )
	{
		string substr;
		getline( ss, substr,':');

		if(index == 1) hour = stoi(substr);
		if(index == 2) minute = stoi(substr);

		index++;
	}
}

//These functions are for sorting
bool compareByWord(Movie* lhs,Movie* rhs) {
    return lhs->MovieName < rhs->MovieName;
}

bool compareByTime(Movie* lhs,Movie* rhs) {
	int hourMovie1,minuteMovie1 = 0;
	int hourMovie2,minuteMovie2 = 0;
	extractTime(lhs->StartingTime,hourMovie1,minuteMovie1);
	extractTime(rhs->StartingTime,hourMovie2,minuteMovie2);
	return hourMovie1*60 + minuteMovie1 < hourMovie2*60 + minuteMovie2;
}

bool compareByNumber(Movie* lhs,Movie* rhs) {
    return lhs->Price < rhs->Price;
}

bool compareByTimeString(string time1,string time2) {
	int hour1,minute1 = 0;
	int hour2,minute2 = 0;
	extractTime(time1,hour1,minute1);
	extractTime(time2,hour2,minute2);
	return hour1*60 + minute1 > hour2*60 + minute2;
}

//Prints All movies in alphabetic order
void printAllMovies(MovieList & movies)
{
	sort(movies.begin(), movies.end(), compareByWord);
	string currentMovie = "";

	for(int i = 0;i < movies.size();i++)
	{
		if(currentMovie != movies[i]->MovieName)
		{
			cout << movies[i]->MovieName << endl;
			currentMovie = movies[i]->MovieName;
		}
	}
}

//This function finds start and end of the same movies in movies vector(sorted by alphabetical order)
/*
* For example:
* movieName = Whiplash
* Movies vector:
* 12 angry men
* 12 angry men
* Whiplash
* Whiplash
* start = 2, end = 3
*/
void findStartAndEnd(MovieList & movies,int &start,int &end,string movieName)
{
	bool find = false;
	for(int i = 0;i < movies.size();i++)
	{
		if(movies[i]->MovieName == movieName && find == false)
		{
			start = i;
			find = true;
		}

		else if(movies[i]->MovieName != movieName && find == true)
		{
			end = i-1;
			find = false;
		}

		else if(find == true && i == movies.size() - 1 && movies[i]->MovieName == movieName)
		{
			end = i;
			find = false;
		}
	}
}

void findOverlappingTime(vector<MovieList  > &plan,int day,vector<MovieList  > &selected)
{
	int start = 0,end = 0;
	string finishingTime;
	for(int i =0;i < plan[day].size();)
	{
		finishingTime = plan[day][i]->FinishingTime;
		//start = i;
		end = i;

		MovieList  selectedMovie;
		selectedMovie.push_back(plan[day][i]);
		for(int j = i+1;j < plan[day].size();j++)
		{
			if(compareByTimeString(finishingTime,plan[day][j]->StartingTime))
			{
				end = j;
			}

			if(plan[day][i]->StartingTime == plan[day][j]->StartingTime) selectedMovie.push_back(plan[day][j]);
		}

		if(selectedMovie.size() == 1)
		{
			selected[day].push_back(selectedMovie[0]);
		}
		else
		{
			
			sort(selectedMovie.begin(), selectedMovie.end(), compareByNumber);
			MovieList  selectedMovie2;
			selectedMovie2.push_back(selectedMovie[0]);
			for(int j = 1;j < selectedMovie.size();j++)
			{
				if(selectedMovie[0]->Price == selectedMovie[j]->Price) selectedMovie2.push_back(selectedMovie[j]);
			}

			if(selectedMovie2.size() == 0)
			{
				selected[day].push_back(selectedMovie[0]);
			}

			else
			{
				sort(selectedMovie2.begin(), selectedMovie2.end(), compareByWord);

				selected[day].push_back(selectedMovie2[0]);
			}
		}

		i = end + 1;
	}
}

void constructPlan(vector<MovieList  > &plan,MovieList & movies,string movieName,vector<MovieList  > &selected)
{
	MovieList  movie;
	/*
	Initializing plan vector
	Row 0: Saturday,1 : Sunday .... 6:Friday*/
	for(int i = 0;i < 7;i++)
	{
		plan.push_back(movie);
	}

	/*
	Initializing selected vector
	Row 0: Saturday,1 : Sunday .... 6:Friday*/
	for(int i = 0;i < 7;i++)
	{
		selected.push_back(movie);
	}

	//First We sort movies vector by Movie Name
	sort(movies.begin(), movies.end(), compareByWord);

	//Then we find where movie x starts and ends in movies vector(sorted by alphabetical order)
	int start,end = 0;
	findStartAndEnd(movies,start,end,movieName);

	for(int i = start; i <= end;i++)
	{
		if(movies[i]->Day == "Saturday") plan[0].push_back(movies[i]);
		else if(movies[i]->Day == "Sunday") plan[1].push_back(movies[i]);
		else if(movies[i]->Day == "Monday") plan[2].push_back(movies[i]);
		else if(movies[i]->Day == "Tuesday") plan[3].push_back(movies[i]);
		else if(movies[i]->Day == "Wednesday") plan[4].push_back(movies[i]);
		else if(movies[i]->Day == "Thursday") plan[5].push_back(movies[i]);
		else if(movies[i]->Day == "Friday") plan[6].push_back(movies[i]);
	}

	//Sorting Plan vector by starting and finishing time
	for(int i = 0; i < 7;i++)
	{
		sort(plan[i].begin(), plan[i].end(), compareByTime);
		findOverlappingTime(plan,i,selected);
	}
}

void constructHTML(vector<MovieList  > selected,string movieName)
{
	ofstream web(movieName+".html");
	ifstream ifs("header.html");

	string header((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));

	web << header;
	int width = 0,left = 0,top = 0;

	int startH = 0,startM = 0;
	int finishH = 0,finishM = 0;
	for(int i = 0; i < selected.size();i++)
	{
		for(int j = 0; j < selected[i].size();j++)
		{
			

			extractTime(selected[i][j]->StartingTime,startH,startM);
			extractTime(selected[i][j]->FinishingTime,finishH,finishM);

			int diffTime = (finishH*60+ finishM)-(startH*60+ startM);
			width = 50 * (diffTime/30);
			left = 100 + 50 * (((startH-8)*60+ startM)/30);
			top = 60 + i*50;
			web << "<div class='record-box' style='" << "width: "<< width <<"px; left: "<< left<<"; top:"<< top <<"; '>"<< selected[i][j]->CinemaName<<"</div>";
		}
	}
	web << "</body></html>";

	ifs.close();
	web.close();
}

//These functions are used for printing table
string charProducer(int quantity,string c)
{
	string s;

	for(int i = 0;i < quantity;i++)
	{
		s +=c;
	}

	return s;
}

string buildLeftAndRightWall(string cinemaName,string line,int startH,int startM,int finishH,int finishM)
{
	int diffTime = (finishH*60+ finishM)-(startH*60+ startM);
	int width =(diffTime/30)*5 - 1;
	int leftDistance =10 + (((startH-8)*60+ startM)/30)*5;

	//Left Wall
	 line[leftDistance - 1] = '|';

	//Cinema Name
	for(int i = 0;i < cinemaName.length();i++)
	{
		line[leftDistance+i+1] = cinemaName[i];	
	}
	//Right Wall
	line[width+leftDistance] = '|';
	return line;
}

string buildUpperWall(string line,int startH,int startM,int finishH,int finishM)
{
	int diffTime = (finishH*60+ finishM)-(startH*60+ startM);
	int width =(diffTime/30)*5 - 1;
	int leftDistance = 10 + (((startH-8)*60+ startM)/30)*5;

	//Left Wall
	line[leftDistance - 1] = '+';
	if(line[leftDistance] != '+') line[leftDistance] = '-';

	for(int i = 0;i < width;i++)
	{
		if(line[leftDistance+i+1] != '+') line[leftDistance+i+1] = '-';	
	}
	//Right Wall
	line[width+leftDistance] = '+';
	return line;
}

//Print Table of movies
void printTable(vector<MovieList  > selected)
{
	vector<string> lines;

	lines.push_back("          08:00               10:00               12:00               14:00               16:00               18:00               20:00               22:00               00:00 \n");

	for(int i = 0; i < 7;i++)
	{
		if(i == 0)
		{
			lines.push_back(charProducer(175," "));
			lines.push_back("Saturday"+charProducer(167," "));
		}
		else if(i == 1)
		{
			lines.push_back(charProducer(175," "));
			lines.push_back("Sunday"+charProducer(169," "));
		}
		else if(i== 2) {
			lines.push_back(charProducer(175," "));
			lines.push_back("Monday"+charProducer(169," "));
		}

		else if(i == 3)
		{
			lines.push_back(charProducer(175," "));
			lines.push_back("Tuesday"+charProducer(168," "));
		}

		else if(i == 4)
		{
			lines.push_back(charProducer(175," "));
			lines.push_back("Wednesday"+charProducer(166," "));
		}

		else if(i == 5)
		{
			lines.push_back(charProducer(175," "));
			lines.push_back("Thursday"+charProducer(167," "));
		}

		else if(i == 6)
		{
			lines.push_back(charProducer(175," "));
			lines.push_back("Friday"+charProducer(169," "));
			lines.push_back(charProducer(175," "));
		}
	}
	for(int i = 0; i < selected.size();i++)
	{
		for(int j = 0; j < selected[i].size();j++)
		{
			int startH = 0,startM = 0;
			int finishH = 0,finishM = 0;

			extractTime(selected[i][j]->StartingTime,startH,startM);
			extractTime(selected[i][j]->FinishingTime,finishH,finishM);

			if((2*i+2) < lines.size())
			{
				lines[2*i+2] = buildLeftAndRightWall(selected[i][j]->CinemaName,lines[2*i+2],startH,startM,finishH,finishM);
			}

			if((2*i+1) < lines.size())
			{
				lines[2*i+1] = buildUpperWall(lines[2*i+1],startH,startM,finishH,finishM);
			}

			if((2*i+3) < lines.size())
			{
				lines[2*i+3] = buildUpperWall(lines[2*i+3],startH,startM,finishH,finishM);

			}
		}

	}

	for(int i = 0; i < lines.size();i++)
	{
		cout <<lines[i] << endl;
	}
}

//This function extracts movie name from GET SCHEDULE <MOVIE NAME>
void extractMovieName(string text,string &movieName)
{
	stringstream ss(text);
	int index = 1;
	while( ss.good() )
	{
		string substr;
		getline( ss, substr,' ');

		if(index > 2)
		{
			if(index > 3) movieName += " ";
			movieName += substr;
		}

		index++;
	}
}

//I use map for solving not constant column
Movie* readMovieInfo(string info,map<string,int> &header)
{
	Movie *newMovie = new Movie;
	stringstream ss(info);

	Movie *next;

	int index = 1;
	
	while( ss.good() )
	{
		string substr;
		getline( ss, substr,',');

		if(header["CinemaName"] == index)newMovie->CinemaName = substr;
		if(header["MovieName"] == index)newMovie->MovieName = substr;
		if(header["Day"] == index)newMovie->Day = substr;
		if(header["StartingTime"] == index)newMovie->StartingTime = substr;
		if(header["FinishingTime"] == index)newMovie->FinishingTime = substr;
		if(header["Price"] == index)newMovie->Price = substr;
		if(header["Hall"] == index)newMovie->Hall = substr;
		index++;
	}
	
	return newMovie;
}

//This function reads schedule file and puts the data on movies vector
void readSchedule(MovieList & movies,string dir)
{
	int lineNumber = 1;
	string line;
	ifstream file(dir);

	map<string,int> header;
	while (getline(file, line))
	{
	
		if(lineNumber > 1)
		{
			movies.push_back(readMovieInfo(line,header));
		}
		else
		{
			stringstream ss(line);
			int index = 1;
	
			while(ss.good())
			{
				string substr;
				getline( ss, substr,',');
				header[substr] = index;
				index++;
			}
		}

		lineNumber++;
	}

	file.close();
}

int main(int argc, char *argv[])
{
	if(argc < 1)
	{
		cout << "Schedule of movies are required!";
	}
	else
	{
		string userCommand;

		MovieList  movies;


		readSchedule(movies,argv[1]);

		getline(cin,userCommand);

		if(userCommand == "GET ALL MOVIES")
		{
			printAllMovies(movies);
		}

		else
		{
			string movieName;
			extractMovieName(userCommand,movieName);

			vector<MovieList  > plan;
			vector<MovieList  > selected;

			constructPlan(plan,movies,movieName,selected);

			constructHTML(selected,movieName);

			printTable(selected);
		}		
	}

	return 0;
}













