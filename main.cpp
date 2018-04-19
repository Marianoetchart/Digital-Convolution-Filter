
// main.cpp
// Task 2 16ELB014: Software Engineering
//
// Created by Mariano  Etchart on 05/02/2017.
// Copyright © 2017 Mariano  Etchart. All rights reserved.
//

// Purpose
// A program to demonstrate the application of a simple digital filter

// Overview
// A sequence of data items and digital filter values need to be entered by the
// user. The application of the filter to the data involves a simple convolution
// operation. The filtered data are stored separately. Data may be saved and stored
// and also loaded.


// Programmer's Opinion: Explicit is better than implicit.

// Other Notes: Throughout the program several design decision were made.
//              One noteable is to use call by reference as opposed to call
//              by value as this is more efficient when dealing with large objects.


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

enum {OK,FILTER_TOO_LONG};


// Data class where data is stored. All members are private.
// Note: In order to override the 'put to' operator, access to private members
// were given to the << operator when called with a Data object by use of the friend keyword.
// Although it does not violate encapsulation entirely, this does seem to weaken it, as

class Data
{
public:
    Data(){Valid = false;Length = 0;}
    void EnterData();
    
    const vector<double> & GetValues() const;
    vector<double> & SetValues();
    
    bool GetValid()const {return this->Valid;}
    void SetValid(bool);
    void resize(unsigned long i){ValuesV.resize(i);}
    
    double & operator[](unsigned long);
    const double & operator[](unsigned long) const;
    
    friend ostream & operator<<(ostream & OutStream, const Data & d );
    
    
private:
    unsigned long Length;
    bool Valid;
    // A vector was used instead of an array as there is no need to have functions for resizing legnth, allocating and clearing memory.
    // A linked list was also considered and would have offered to never have to re-allocate memory just for an additional element which is cheap,
    // nevertheless, a vector was chosen because it is able to pre-allocated memory and fit the program design. Also offered built in
    // methods for resizing and getting the size of it.
    vector <double> ValuesV;
};


// Overiding [] operator in order to be able to index the Data object and access the index of vector<double> behind the code.
double & Data::operator[](unsigned long i){return this->ValuesV[i];}
const double & Data::operator[](unsigned long i)const{return this->ValuesV[i];}


// It was a design choice to have a function accessing the ValuesV vector with
// free access to all member functions, and another const function with only const accesss,
// returning a const vector. A mutator and an accessor.

const vector<double> & Data::GetValues() const{return this->ValuesV;}
vector<double> & Data::SetValues(){return this->ValuesV;}

void Data::SetValid(bool newValid ){this->Valid= newValid;} // Sets length of data object


void Data::EnterData()
{
    // getting the number of data values from the user
    cout << "Enter the number of data values you want to have: ";
    cin >>this->Length;
    
    // Exception handling to try allocate memory
    try {ValuesV.resize(this->Length);}
    catch(exception & e) // the correct exception will be prompted, not specified
    {
        cerr << "UNABLE TO ALLOCATE MEMORY!" << endl;
        exit(1);
    }

    
    // obtain all of the data values
    cout << "\nPress Enter key in between values."<<endl;
    for (unsigned long i = 0; i < this->Length;i++)
    {
        cout << "Enter Value:" << endl;
        cin >> this->ValuesV[i];
        cout << "Value Entered Successfully.\n\n";
    }
    this->Valid = true;
}



// Filter class for entereing the filter values and applying the filter.

class Filter
{
public:
    Filter(){Length =0;Valid = false;};
    void EnterFilter();
    int ApplyFilter( const Data &, Data &);
    
    bool GetValid()const{return this->Valid;}
    void resize(unsigned long i){ValuesV.resize(i);}

    //Accessor and mutator functions for ValuesV vector
    const vector<double> & GetValues() const{return ValuesV;}
    vector<double> & SetValues(){return ValuesV;}
    
    // allowing private access for << operator on this case only.
    friend ostream & operator<<(ostream & OutStream, const Filter & d ); // allowing the << operator access to Filter
    
private:
    vector<double> ValuesV;
    unsigned long Length;  // number of data values
    bool Valid;   // true if the data values have been obtained
};


void Filter::EnterFilter()
{
    
    // getting the number of filter values from the user
    cout << "Enter the number of filter values you want to have: " << endl;
    cin >> this->Length;
    
    // Exception handling to try allocate memory
    try {ValuesV.resize(this->Length);}
    catch(exception & e) // the correct exception will be prompted, not specified
    {
        cerr << "UNABLE TO ALLOCATE MEMORY!" << endl;
        exit(1);
    }
    
    // obtain all of the filter values
    cout << "\nPress Enter key in between values."<<endl;
    for (unsigned long i = 0; i < this->Length; ++i)
    {
        cout<<"Enter Value:"<<endl;
        cin >> this->ValuesV[i];
        cout << "Value Entered Successfully.\n\n";
    }
    this->Valid = true;
}


// Fucntion that takes a const reference to input data object and a reference to filtered data object
// NOTE: The subscripting operator '[]' has been overriden in order to call a mutator function and
// return the indexed vector element reference

int Filter::ApplyFilter( const Data & DataIn, Data & FilteredData)
{
    // return an error if the filter is longer than the data
    if (this->Length > DataIn.GetValues().size()) return FILTER_TOO_LONG;
    else
        {
        // else try allocate memory with exception handling
        try{FilteredData.SetValues().resize(DataIn.GetValues().size() - this->Length + 1); }
        catch(exception &e) // the correct exception will be prompted, not specified
            {
                cerr << "UNABLE TO ALLOCATE MEMORY!" << endl;
                exit(1);
            }
        }
    
    // Applying convolution
    for(unsigned long output_index = 0;output_index < FilteredData.GetValues().size(); output_index++)
    {
        FilteredData[output_index] = 0;
        for(unsigned long filter_index = 0 ; filter_index < this->Length ; filter_index++)
        {
            FilteredData[output_index] += DataIn[output_index + filter_index]* this->ValuesV[filter_index] ;
        }
    }
    return OK;
}


// Menu Class where data is displayed, saved and loaded.

class Menu
{
    public:
        Menu(){MenuStart();}
        int MenuStart();
    
        void DisplayData( const Filter &, const Data &, const Data  &);
        void SaveData(const Filter &,const Data &,const Data &);
        void LoadData(Filter &Filter1,Data &OriginalData,Data &FilteredData);
    
    private:
        char UserInput;
};


int Menu::MenuStart()
{
    // Instantiate  the filter, orginal data, and filtered data
    
    Filter Filter1;
    Data OriginalData;
    Data FilteredData;
    
    // loop until the user wishes to exit
    while (1) {
        
        // show the menu of options
        cout << endl;
        cout << "Filter Menu" << endl;
        cout << "-----------" << endl;
        cout << "1. Enter data for filtering" << endl;
        cout << "2. Enter filter values" << endl;
        cout << "3. Apply filter" << endl;
        cout << "4. Display filtered data" << endl;
        cout << "5. Save the current data" << endl;
        cout << "6. Load previously saved data" << endl;
        cout << "7. Exit from the program" << endl << endl;
        
        // get the user's choice
        cout << "Enter your option: ";
        cin >> UserInput;
        cout << endl;
        
        // act on the user's input
        switch(UserInput) {
            case '1':
                OriginalData.EnterData();
                FilteredData.SetValid(false);
                break;
                
            case '2':
                Filter1.EnterFilter();
                FilteredData.SetValid(false);
                break;
                
            case '3':
                if (Filter1.GetValid() == true && OriginalData.GetValid() == true && FilteredData.GetValid() == false) {
                    if (Filter1.ApplyFilter(OriginalData,FilteredData) == FILTER_TOO_LONG) {
                        cout << "The filter must not be longer than the data" << endl;
                    }
                    else {
                        FilteredData.SetValid(true);
                        cout << "Filter applied" << endl;
                    }
                }
                break;
                
            case '4':
                if (Filter1.GetValid() == true && OriginalData.GetValid() == true && FilteredData.GetValid() == true) {
                    DisplayData(Filter1,OriginalData,FilteredData);
                }
                else {
                    cout << "Data have not yet been filtered" << endl;
                }
                break;
                
            case '5':
                SaveData(Filter1,OriginalData,FilteredData);
                break;
                
            case '6':
                LoadData(Filter1,OriginalData,FilteredData);
                break;
                
            case '7':
                Filter1.SetValues().clear();
                OriginalData.SetValues().clear();
                FilteredData.SetValues().clear();
                
                return 0;
                break;
                
            default:
                cout << "Invalid entry" << endl << endl;
                break;
        }
    }
}



// Display input data, filter values and output data
void Menu::DisplayData(const Filter &Filter1,
                       const Data &OriginalData,
                       const Data  &FilteredData)
{


    // display all of the input data values
    cout << "\nInput Values" << endl;
    cout << "-----------" << endl;
    cout << OriginalData;
    
    // display all of the filter values
    cout << "\nFilter Values" << endl;
    cout << "-----------" << endl;
    cout << Filter1;
    
    // display all of the data output values
    cout << "\nCorrelation Output Values" << endl;
    cout << "-----------" << endl;
    cout << FilteredData;
}



// Saving data function. Opens new file with pointer to file. Writes, and closes file.
// The length of each vector is first stored in order to be used when loading.
// In order to differentiate files and not overwrite on the OS, time.h could be used to attach a string of the current time to the file name
// and later be loaded with the path. Nevertheless, that is out of the scope of this class and takes more memory.


void Menu::SaveData(const Filter &Filter1,
                    const Data &OriginalData,
                    const Data  &FilteredData)
{
    fstream FileP; // pointer to file
    
    FileP.open("Saved_Data.txt", ios::out);

    // Checking if pointer is valid before writing
    if (FileP.good() == false)
    {
        cerr << "ERROR: COULD NOT CREATE FILE! EXITING." << endl; // possible memory shortage
        exit(1);
    }
    else
    {
        // Writing length of vectors into file
        FileP << OriginalData.GetValues().size()<<endl ;
        FileP << Filter1.GetValues().size()<<endl;
        FileP << FilteredData.GetValues().size()<<endl;
        
        // Writing vectors into file
        FileP << OriginalData;
        FileP << Filter1;
        FileP << FilteredData;
        
        cout << "Data Saved" << endl;
    }
    FileP.close();
}


// Load function
void Menu::LoadData( Filter &Filter1,
                     Data &OriginalData,
                     Data  &FilteredData)
{
    // Unsigned long used to store length of all vectors
    unsigned long LengthOD, LengthF, LengthFD;
    //cout << "Please enter path of txt file you want to store: " <<endl;
    fstream FileP;
    
    FileP.open("Saved_Data.txt", ios::in);
    
    // checking if pointer is valid
    if (FileP.good() == false)
    {
        cerr << "ERROR: COULD NOT LOAD FILE!" << endl; // possible memory shortage
        exit(1);
    }
    else
    {   // reading lengths and storing
        FileP >> LengthOD;
        FileP >> LengthF;
        FileP >> LengthFD;
        
        //resizing vectors according to file data
        OriginalData.resize(LengthOD);
        Filter1.resize(LengthF);
        FilteredData.resize(LengthFD);
        
        // reading from filestream, and storing elements in respective vectors. This is acheivable since input is terminated by white space
        for (unsigned long i = 0; i < LengthOD; ++i)
        {
            FileP >> OriginalData.SetValues()[i];
        }
        for (unsigned long i = 0; i < LengthF; ++i)
        {
            FileP >> Filter1.SetValues()[i];
        }
        for (unsigned long i = 0; i < LengthFD; ++i)
        {
            FileP >> FilteredData.SetValues()[i];
        }
    }
    FileP.close();
}

// Overiding the 'put to' operator to print Data and Filter objects.
// After looking at this outside of this class, I iterate through the vector with a constant iterator
// and place the value of iterator in the outstream

ostream & operator<<(ostream & OutStream, const Data & d)
{
    for (vector<double>::const_iterator i = d.GetValues().begin() ; i < d.GetValues().end(); i++)
    {
        OutStream <<  *i << endl;
    }
    return OutStream;
}

ostream & operator<<(ostream & OutStream, const Filter & d)
{
    for (vector<double>::const_iterator i = d.GetValues().begin() ; i < d.GetValues().end(); i++)
    {
        OutStream <<  *i << endl;
    }
    return OutStream;
}


// main program.
int  main()
{
    Menu Menu1;
    return 0;
}

















