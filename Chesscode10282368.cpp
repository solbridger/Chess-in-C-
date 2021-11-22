// PHYS 30672 OOP in C++
// Final Project - Chess game

#include <iostream>
#include <iomanip>
#include <ostream>
#include <vector>
#include <map>
#include <limits>
#include <numeric>

// class for defining generic structure of piece

class piece
{
  protected:
    char colour;
  public:
	piece(char colour_in) : colour(colour_in) {}
	virtual ~piece() {}
	virtual char get_piece() = 0;
    virtual int get_val() = 0;
	char get_colour() 
    {
		return colour;
	}
     // algorithm for move dynamics
    virtual bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) = 0;

    // algorithm for calling relevant piece legal move test
    bool is_move_legal(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8])
    {
		piece* chosen_dest = the_board[dest_rank][dest_file];
		if((chosen_dest == 0) || (colour != chosen_dest->get_colour())) // piece must move and cannot capture your own piece
        {
			return legal_square_test(current_rank, current_file, dest_rank, dest_file, the_board);
		}

		return false;
	}
};

class pawn : public piece // all pieces must inherit 'piece' class structure
{
  public:
    pawn(char colour_in) : piece(colour_in) {}
    ~pawn() {}
  private:
    // "override" used to help compiler validation
    virtual char get_piece() override 
    {
        return 'P';
    }
    virtual int get_val() override
    {
        return 1; // material value of piece
    }
    bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) override
    {
        piece* chosen_dest = the_board[dest_rank][dest_file];

        if(chosen_dest == 0) 
        {
            if(current_file == dest_file) // move forward
            {
                if(get_colour() == 'w')
                {
                    if(dest_rank == current_rank +1) 
                    {
                        return true;
                    }
                    if((dest_rank == current_rank +2) && current_rank == 1) // two squares forward possible if first move
                    {
                        return true;
                    }
                }
                if(get_colour() == 'b')
                {
                    if(dest_rank == current_rank -1)
                    {
                        return true;
                    }
                    if((dest_rank == current_rank -2) && current_rank == 6) 
                    {
                        return true;
                    }
                }
            } return false;
        }

        else // destination square occupied by opposite colour piece
        {
            if((dest_file == current_file -1) || (dest_file == current_file +1)) // capture
            {
                if(get_colour() == 'w') 
                {
					if(dest_rank == current_rank +1) 
                    {
						return true;
					}
				} 
                else 
                {
					if(dest_rank == current_rank -1) 
                    {
						return true;
					}
				}
            } return false;
        }

        return false; 
    }
};

class knight : public piece
{
  public:
    knight(char colour_in) : piece(colour_in) {}
    ~knight() {}
  private:
    virtual char get_piece() override
    {
        return 'N';
    }
    virtual int get_val() override
    {
        return 3;
    }
    bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) override
    { // maximally there are 8 allowed moves .: 8 conditions
        if( (dest_rank == current_rank -1 && dest_file == current_file +2) ||
            (dest_rank == current_rank -1 && dest_file == current_file -2) || 
            (dest_rank == current_rank +1 && dest_file == current_file +2) ||
            (dest_rank == current_rank +1 && dest_file == current_file -2) )
        {
            return true;
        }
        if( (dest_rank == current_rank +2 && dest_file == current_file +1) ||
            (dest_rank == current_rank +2 && dest_file == current_file -1) || 
            (dest_rank == current_rank -2 && dest_file == current_file +1) ||
            (dest_rank == current_rank -2 && dest_file == current_file -1) )
        {
            return true; 
        }

        return false;
    }
};

// class templates for commonly used conditions for allowed piece moves

template <class c_type> c_type delta_condition(c_type dest, c_type curr)
{
    return int (dest - curr > 0) ? 1 : -1; // condition for diagonal iteration to be +ve/ -ve
}

template <class c_type> c_type abs_diff_of(c_type dest, c_type curr)
{
    int calc = std::abs(dest - curr);
    return calc; 
}

class bishop : public piece
{
  public:
    bishop(char colour_in) : piece(colour_in) {}
    ~bishop() {}
  private:
    virtual char get_piece() override
    {
        return 'B';
    }
    virtual int get_val() override
    {
        return 3;
    }
    bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) override
    {
        int delta_file = delta_condition(dest_file, current_file);
        int delta_rank = delta_condition(dest_rank, current_rank);

        if(abs_diff_of(dest_rank, current_rank) == abs_diff_of(dest_file, current_file)) // diagonality condition
        {
            int check_rank, check_file;
            // direction of diagonal squares determined by delta_rank/file 
            for(check_rank = current_rank + delta_rank, check_file = current_file + delta_file;
                check_rank != dest_rank; check_rank += delta_rank, check_file += delta_file) // iterating through diagonal
            {
                if(the_board[check_rank][check_file] != 0)
                {
                    return false;
                }
            }
            return true;
        }

        return false;
    }

};

class rook : public piece
{
  public:
    rook(char colour_in) : piece(colour_in) {}
    ~rook() {}
  private:
    virtual char get_piece() override
    {
        return 'R';
    }
    virtual int get_val() override
    {
        return 5;
    }
    bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) override
    {
        int delta_rank = abs_diff_of(dest_rank, current_rank);
        int delta_file = abs_diff_of(dest_file, current_file);

        int delta_rank_check = delta_condition(dest_rank, current_rank);
        int delta_file_check = delta_condition(dest_file, current_file);

        if(delta_rank == 0)
        {
            int check_rank, check_file;
            for(check_file = current_file + delta_file_check; check_file != dest_file;  check_file += delta_file_check) // iterate along file 
            {
                if(the_board[current_rank][check_file] != 0) 
                {
                    return false;
                }
            }

            return true;
        }

        if(delta_file == 0)
        {
            int check_rank, check_file;
            for(check_rank = current_rank + delta_rank_check; check_rank != dest_rank; check_rank += delta_rank_check) // iterate along rank
            {
                if(the_board[check_rank][current_file] != 0)
                {
                    return false;
                }
            }

            return true;

        }

        return false;
    }

};

class queen : public piece 
{
  public:
    queen(char colour_in) : piece(colour_in) {}
    ~queen() {}
  private:
    virtual char get_piece() override
    {
        return 'Q';
    }
    virtual int get_val() override
    {
        return 9;
    }
    bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) override
    { // queen moves are a combination of rook and bishop moves 
        int delta_rank = abs_diff_of(dest_rank, current_rank);
        int delta_file = abs_diff_of(dest_file, current_file);

        if(delta_file == delta_rank) // diagonal moves
        {
            int delta_file_diag = delta_condition(dest_file, current_file);
            int delta_rank_diag = delta_condition(dest_rank, current_rank);
            int check_rank, check_file;
            
            for(check_rank = current_rank + delta_rank_diag, check_file = current_file + delta_file_diag;
                check_rank != dest_rank; check_rank += delta_rank_diag, check_file += delta_file_diag)
            { 
                if(the_board[check_rank][check_file] != 0)
                {
                    return false;
                }
            }
            return true;
        }

        // rook moves

        if(delta_rank == 0)
        {
            int check_rank, check_file;
            int delta_file_check = delta_condition(dest_file, current_file);

            for(check_file = current_file + delta_file_check; check_file != dest_file;  check_file += delta_file_check) // iterate along rank 
            {
                if(the_board[current_rank][check_file] != 0) 
                {
                    return false;
                }
            }

            return true;
        }

        if(delta_file == 0)
        {
            int check_rank, check_file;
            int delta_rank_check = delta_condition(dest_rank, current_rank);

            for(check_rank = current_rank + delta_rank_check; check_rank != dest_rank; check_rank += delta_rank_check) // iterate along file
            {
                if(the_board[check_rank][current_file] != 0)
                {
                    return false;
                }
            }

            return true;

        }

        return false;
    } 
};

class king : public piece
{
  public:
    king(char colour_in) : piece(colour_in) {}
    ~king() {}
  private:
    virtual char get_piece() override
    {
        return 'K';
    }
    virtual int get_val() override
    {
        return 0;
    }
    bool legal_square_test(int current_rank, int current_file, int dest_rank, int dest_file, piece* the_board[8][8]) override
    {
        if(((dest_rank - current_rank) >= -1) && ((dest_rank - current_rank) <= 1) &&
           ((dest_file - current_file) >= -1) && ((dest_file - current_file) <= 1))
        {
            return true;
        }

        return false;
    }
};

// class to define board structure, board layout and general move checks

class board
{
  private:
    std::string w_sq{"\u2B1C"}; // white square
    std::string b_sq{"\u2B1B"};
  public:
    board() 
    {
		for(int i_rank{}; i_rank < 8; ++i_rank) 
        {
			for (int i_file{}; i_file < 8; ++i_file) 
            {
				the_board[i_rank][i_file] = 0; // initialise all squares to 0 
			}
		}
        // add white pieces
		for (int i_file{}; i_file < 8; ++i_file)
        {
			the_board[1][i_file] = new pawn('w'); 
		}
		the_board[0][0] = new rook('w');
		the_board[0][1] = new knight('w');
		the_board[0][2] = new bishop('w');
		the_board[0][3] = new king('w');
		the_board[0][4] = new queen('w');
		the_board[0][5] = new bishop('w');
		the_board[0][6] = new knight('w');
		the_board[0][7] = new rook('w');
		// add black pieces
		for(int i_file{}; i_file < 8; ++i_file)
        {
			the_board[6][i_file] = new pawn('b');
		}
		the_board[7][0] = new rook('b');
		the_board[7][1] = new knight('b');
		the_board[7][2] = new bishop('b');
		the_board[7][3] = new king('b');
		the_board[7][4] = new queen('b');
		the_board[7][5] = new bishop('b');
		the_board[7][6] = new knight('b');
		the_board[7][7] = new rook('b');

    }
	~board() 
    {
		for (int i_rank{}; i_rank < 8; ++i_rank) 
        {
			for (int i_file{}; i_file < 8; ++i_file) 
            {
				delete the_board[i_rank][i_file]; // clean up
				the_board[i_rank][i_file] = 0;
			}
		}
	}

     // Use this set up for testing checkmate i.e. playing (R) a1 -> a8
		/*
		the_board[0][0] = new rook('w');
		the_board[0][3] = new king('w');
		the_board[0][4] = new queen('w');
        the_board[6][2] = new pawn('b');
        the_board[6][3] = new pawn('b');
        the_board[6][4] = new pawn('b');
		the_board[7][3] = new king('b');
		the_board[7][4] = new queen('b');
	   */

    void print()
    {   
        using namespace std;
        int sq_height{5}, sq_width{6}; // algorithm won't work correctly if these values are changed
        // print board file by file
        if(sq_height > 0 && sq_width > 0)
        {
            for(int file_sq{}; file_sq < 8*sq_width; file_sq++)
            {
                for(int rank_sq{}; rank_sq < 8*sq_height; rank_sq++)
                {
                    int r = rank_sq/sq_height; int f = file_sq/sq_width; // index transformation between 8x8 and 8*sq_width*8*sq_height
                    if(the_board[r][f] != 0 && (rank_sq % 5) == 2 && (((file_sq % 3) == 2 && (file_sq % 2) == 0) 
                    || ((file_sq % 2) == 1 && (file_sq % 3) == 0)))// if square contains pieces and satisfies piece/colour coords
                    {
                        if((file_sq % 3) == 2 && file_sq % 2 == 0) // on 5x6 board - file of colour (of piece) squares are 2,8,14,20,...
                        {
                            cout<<the_board[r][f]->get_colour()<<" "; 
                        }
                        else // on 5x6 board - file of pieces squares are 3,9,15,21,...
                        {
                            if(file_sq % 2 == 1 && file_sq % 3 == 0 && the_board[r][f] != 0)
                            {
                                cout<<the_board[r][f]->get_piece()<<" ";
                            }
                        }
                    }
                    else 
                    {
                        int w = 5;
                        if((r+f) % 2 == 0) // condition for square to be white
                        {
                            cout<<w_sq;  
                            if(rank_sq == 8*sq_height -1) // print file lettering at end of file
                            {
                                if(file_sq == 8)
                                {
                                    cout<<std::setw(w)<<"B"; 
                                }
                                if(file_sq == 20)
                                {
                                    cout<<std::setw(w)<<"D"; 
                                }
                                if(file_sq == 32)
                                {
                                    cout<<std::setw(w)<<"F"; 
                                }
                                if(file_sq == 44)
                                {
                                    cout<<std::setw(w)<<"H"; 
                                }
                            }
                        }
                        
                        else
                        {
                            if((r+f) % 2 == 1) // true for all black squares
                            {
                                cout<<b_sq;
                                if(rank_sq == 8*sq_height -1)
                                {
                                    if(file_sq == 2)
                                    {
                                        cout<<std::setw(w)<<"A"; 
                                    }
                                    if(file_sq == 14)
                                    {
                                        cout<<std::setw(w)<<"C"; 
                                    }
                                    if(file_sq == 26)
                                    {
                                        cout<<std::setw(w)<<"E"; 
                                    }
                                    if(file_sq == 38)
                                    {
                                        cout<<std::setw(w)<<"G"; 
                                    }
                                }
                            }
                        }
                    }
                }  cout<<endl; //print each line seperately
            } 
        }

        if(sq_height <=0 || sq_width <=0)
        {
            cerr<<"Both the square height and width must be > 0."<<endl;
        }
        std::cout<<'\n';

        // for printing board rank names 1-8 (at bottom)
        for(int k{1}; k <= 8; k++)
        {   
            cout<<"    ";
            int width = 2*sq_height -1; 
            cout<<k<<setw(width);
        }
        cout<<endl;
    }

    void print_material_val() // material value summation - a somewhat primitive definition of who's winning 
    {
        int white_sum{}; int black_sum{};
        std::vector<int> material_sum_white;
        std::vector<int> material_sum_black;

        for(int i{}; i < 8; i++)
        {
            for(int j{}; j < 8; j++)
            {
                piece* piece_val = the_board[i][j]; 

                if(piece_val != 0)
                {
                    if(piece_val->get_colour() == 'w')
                    {
                        material_sum_white.push_back(piece_val->get_val());
                    }

                    if(piece_val->get_colour() == 'b')
                    {
                        material_sum_black.push_back(-1 * piece_val->get_val()); // *-1 as material is equal initially by definition
                    }
                }
            }
        }

        for(std::vector<int>::iterator it = material_sum_white.begin(); it != material_sum_white.end(); ++it)
        {
            white_sum += *it;
        }

        for(std::vector<int>::iterator it = material_sum_black.begin(); it != material_sum_black.end(); ++it)
        {
            black_sum += *it;
        }

        int material_sum = black_sum + white_sum;

        if(material_sum == 0)
        {
            std::cout<<"\nThe material on the board is currently equal!"<<std::endl;
        }
        
        if(material_sum > 0)
        {
            std::cout<<"\nThe total material sum is = +"<<material_sum<<std::endl;
            std::cout<<"White has the advantage currently!"<<std::endl;
        }

        if(material_sum < 0)
        {
            std::cout<<"\nThe total material sum is = "<<material_sum<<std::endl;
            std::cout<<"Black has the advantage currently!"<<std::endl;
        }

    }

    bool in_check(char piece_colour)
    {

        int king_rank, king_file;
        for(int i{}; i < 8; i++) // loops for finding location of king
        {
            for(int k{}; k < 8; k++)
            {
                piece* king_loc = the_board[i][k];
                if(king_loc != 0)
                {
                    if(king_loc->get_piece() == 'K' && king_loc->get_colour() == piece_colour)
                    {
                        king_rank = i; king_file = k;
                    }
                }
            }
        }

        for(int rank_m{}; rank_m < 8; rank_m++) // loops for determining if opponents piece can take the king
        {
            for(int file_n{}; file_n < 8; file_n++)
            {
                piece* piece_check_loc = the_board[rank_m][file_n];
                if(piece_check_loc != 0)
                {
                    if(piece_check_loc->get_piece() != 0)
                    {
                        if(piece_check_loc->get_colour() != piece_colour)
                        {
                            if(piece_check_loc->is_move_legal(rank_m, file_n, king_rank, king_file, the_board))
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;

    }

    bool can_move(char piece_colour)
    {
        for(int initial_file{}; initial_file < 8; initial_file++)
        {
            for(int initial_rank{}; initial_rank < 8; initial_rank++)
            {
                piece* temp_i = the_board[initial_rank][initial_file];

                if(temp_i != 0)
                {
                    if(temp_i->get_colour() == piece_colour)
                    {
                        for(int end_file{}; end_file < 8; end_file++)
                        {
                            for(int end_rank{}; end_rank < 8; end_rank++)
                            {
                                if(temp_i->is_move_legal(initial_rank, initial_file, end_rank, end_file, the_board))
                                {
                                    piece* temp_dest = the_board[end_rank][end_file];
                                    the_board[end_rank][end_file] = the_board[initial_rank][initial_file]; // piece moves to dest square
                                    the_board[initial_rank][initial_file] = 0; // initial square no longer occupied

                                    if(in_check(piece_colour) == false)
                                    {
                                        // move is valid
                                        the_board[initial_rank][initial_file] = temp_i;
                                        the_board[end_rank][end_file] = temp_dest;
                                        return true;
                                    }
                                    else
                                    {
                                        the_board[initial_rank][initial_file] = temp_i;
                                        the_board[end_rank][end_file] = temp_dest;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    piece* the_board[8][8]; // access to the board data
};

// template classes for transforming character (E2) input to coords on [8][8] board 

template <class c_type> c_type coord_transf(c_type rank, c_type file)
{
    return int (rank*10 + file); 
}

template <class c_type> c_type digit_rank(c_type move)
{
    return int ((move/10) -1);
}

template <class c_type> c_type digit_file(c_type move)
{
    return int ((move % 10) -1);
}

// class for handling structure of game play

class chess
{
  private:
	board game_board;
	char player_turn;
  public:
	chess() : player_turn('w') {}
	~chess() {}
	void play() 
    {
		do 
        {
			get_next_move(game_board.the_board);
			alternate_turn();
		} 

        while(is_game_over() == false);
		game_board.print();
        game_board.print_material_val();
	}
    
    int convert(char in) // allow for algebraic notation to be inputted
    {
        const std::string alpha = "ABCDEFGH";
        std::map<char,int> alpha_map;
        for(int i{}; i < 8; i++)
        {
            alpha_map[alpha[i]] = i+1; // maps a value of A = 1, B = 2, ...
        
            if(std::toupper(in) == alpha[i]) // letter matches
            {
                int val{};
                val = alpha_map[alpha[i]];
                return val;
            }
        }
        return 0;
    }

    // algorithm to prevent illegal move (player not responding to check/ putting themselves in check)

    bool dest_not_in_check(int current_rank, int current_file, int dest_rank, int dest_file, piece* check_board[8][8])
    {
        piece* temp_i = check_board[current_rank][current_file];
        piece* temp_d = check_board[dest_rank][dest_file];
        
        if(temp_i->legal_square_test(current_rank, current_file, dest_rank, dest_file, game_board.the_board) == true)
        {
            if(game_board.in_check(player_turn) == true)
            {
                std::cout<<"\nYou are in check!"<<std::endl;
                check_board[current_rank][current_file] = temp_i;
                check_board[dest_rank][dest_file] = temp_d;
                return false;
            }

        return true;
        }

        return false;

    }

	void get_next_move(piece* move_board[8][8]) // defines move interface 
    {
        using namespace std;
		bool valid_move = false;
        while(!valid_move)
        {
			game_board.print();
            game_board.print_material_val();

			cout<<'\n'<<player_turn<<" player's turn: "<<endl;
            int start_rank, start_file, end_rank, end_file, start_input_rank, end_input_rank;
            char start_input_file, end_input_file;

            string file_error_msg{"Input invalid! File must be a letter.\n"};
            string rank_error_msg{"Input invalid! Rank must be an integer.\n"};

            // prevent errors in the case of invalid input
            try
            {
                cout<<"\nEnter piece starting position - A2,E2,C3 etc : "<<std::endl;
                cout<<"You can enter the file/ rank seperately by pressing enter inbetween"<<endl;
                cin >> start_input_file;
                if(cin.fail())
                {
                    throw invalid_argument(file_error_msg);
                }

                cin >> start_input_rank;
                if(cin.fail())
                {
                    throw invalid_argument(rank_error_msg);
                }

                cout<<"\nto: "<<endl;
                cout<<"Enter piece end position: "<<endl;

                cin >> end_input_file;
                if(cin.fail())
                {
                    throw invalid_argument(file_error_msg);
                }

                cin >> end_input_rank;
                if(cin.fail()) 
                {
                    throw invalid_argument(rank_error_msg);
                }
            }
            catch(int &start_input_file)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');//Rejects string of max length
                cin>>start_input_file;
            }
            catch(char &start_input_rank)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin>>end_input_file;
            }
            
            // // Input to board coordinate conversion

            int start_move_file = convert(start_input_file);
            
            int start_move = coord_transf(start_input_rank, start_move_file); 

            start_rank = digit_rank(start_move);
			start_file = digit_file(start_move);

            int end_move_file = convert(end_input_file); 

            int end_move = coord_transf(end_input_rank, end_move_file); 

			end_rank = digit_rank(end_move); 
			end_file = digit_file(end_move); 
            
			// check within board & position of board has changed
			if ((start_rank >= 0 && start_rank < 8) && (start_file >= 0 && start_file < 8) &&
			   (end_rank >= 0 && end_rank < 8) && (end_file >= 0 && end_file < 8)) 
            {
                piece* temp_start = move_board[start_rank][start_file];
				// test that square contains piece and colour is correct
				if(temp_start != 0 &&  temp_start->get_colour() == player_turn) 
                {
                    // cannot move into check
                    if(dest_not_in_check(start_rank, start_file, end_rank, end_file, game_board.the_board) == true)
                    {    
                        // check valid destination
                        if(temp_start->is_move_legal(start_rank, start_file, end_rank, end_file, game_board.the_board) == true) 
                        {
                            // move
                            piece* temp_end = move_board[end_rank][end_file];
                            move_board[end_rank][end_file] = move_board[start_rank][start_file];
                            move_board[start_rank][start_file] = 0; // square no longer occupied

                            if(!game_board.in_check(player_turn)) 
                            {
                                valid_move = true;
                            }
                           
                            if(valid_move == true)
                            {
                                if(temp_start->get_piece() == 'P')
                                {
                                    if(temp_start->get_colour() == 'w')
                                    {
                                        if(end_rank == 7)
                                        {
                                            move_board[7][end_file] = 0; // piece captured must be removed from game
                                            cout<<"Promote your pawn, enter which piece - either Q, R, B or N: "<<endl;
                                            char promote_piece;
                                            cin>>promote_piece;
                                            char pro = toupper(promote_piece);
                                            if(pro == 'Q')
                                            {
                                                move_board[7][end_file] = new queen('w');
                                            }
                                            if(pro == 'R')
                                            {
                                                move_board[7][end_file] = new rook('w');
                                            }                  
                                            if(pro == 'B')
                                            {
                                                move_board[7][end_file] = new bishop('w');
                                            }
                                            if(pro == 'N')
                                            {
                                                move_board[7][end_file] = new knight('w');
                                            }
                                            else{}
                                            cin.clear();
                                        }
                                    }

                                    else // black
                                    {
                                        if(end_rank == 0)
                                        {
                                            move_board[0][end_file] = 0;
                                            cout<<"Promote your pawn, enter which piece - either Q, R, B or N: "<<std::endl;
                                            char promote_piece;
                                            cin>>promote_piece;
                                            char pro = toupper(promote_piece);
                                            if(pro == 'Q')
                                            {
                                                move_board[0][end_file] = new queen('b');
                                            }
                                            if(pro == 'R')
                                            {
                                                move_board[0][end_file] = new rook('b');
                                            }                  
                                            if(pro == 'B')
                                            {
                                                move_board[0][end_file] = new bishop('b');
                                            }
                                            if(pro == 'N')
                                            {
                                                move_board[0][end_file] = new knight('b');
                                            }
                                            else{}
                                            cin.clear();
                                        }
                                    }
                                }
                            }
                            
                            if(valid_move = true) // move can only be viewed if legal
                            {
                                cout<<"Would you like to view the previous move? [y/n]"<<endl;
                                char answer;
                                cin>>answer;
                                if(tolower(answer) == 'y')
                                {
                                    cout<<"Previous move: "<<endl;
                                    cout<<"(Where the first and final values represent the move transformation {start}{}{}{end})"<<endl;
                                    vector<char> char_moves;
                                    vector<int> int_moves;
                                    char_moves.push_back(start_input_file);
                                    char_moves.push_back(end_input_file);
                                    int_moves.push_back(start_input_rank);
                                    int_moves.push_back(end_input_rank);
                                    vector<char>::iterator char_it;
                                    vector<int>::iterator int_it;
                                    for(auto char_it = char_moves.begin(); char_it != char_moves.end(); char_it++)
                                    {
                                        for(auto int_it = int_moves.begin(); int_it != int_moves.end(); int_it++)
                                        {
                                            cout<<"{"<<*char_it<<*int_it<<"}";
                                        }
                                    }cout<<endl;
                                    delete temp_end;
                                }
                                else
                                {
                                    delete temp_end;
                                    cin.clear();
                                }
                            }
                            
                            else 
                            { // undo previous move, if it violates checks
                                move_board[start_rank][start_file] = move_board[end_rank][end_file];
                                move_board[end_rank][end_file]	= temp_end;
                            }
                        }
                    }
				}
			}
            
			if(!valid_move) 
            {
				cout<<"Invalid move."<<endl;
                cout<<"Enter a valid move."<<endl;
			}
		} 
	}

	void alternate_turn() 
    {
		player_turn = (player_turn == 'w') ? 'b' : 'w';
	}

    void final_output() 
    {
        game_board.print();
        game_board.print_material_val();
        std::cout<<"\nThank you for playing!"<<std::endl;
    }

    void clean_up() // deletes data no longer needed
    {
        for(int i{}; i<8; i++)
        {
            for(int j{}; j<8; j++)
            {
                delete game_board.the_board[i][j];
            }
        }
    }

    void control() // organises and controls sequence of exiting game
    {
        final_output();
        clean_up();
        exit(1);
    }

	bool is_game_over() // algorithm to ensure game is terminated if it ends
    {
		if(game_board.can_move(player_turn) == false) 
        {
			if(game_board.in_check(player_turn) == true) 
            {
				alternate_turn(); // player that can move wins
				std::cout << "\nCheckmate, the " << player_turn << " player wins!" << std::endl;
                control();
			} 

            else 
            {
				std::cout << "\nThe game has ended in a stalemate, it's a draw." << std::endl;
                control();
			}
		}
		return !game_board.can_move(player_turn);
	}
};

int main()
{   
    std::cout<<"Welcome to my chess program!\n"<<std::endl;
    chess game;
    game.play();
    return 0;
}