#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
using namespace std;

typedef vector<bool> Deck;
typedef vector<int> Cards;
struct Hand{
    int player;
    int royal_flush;
    Cards cards;
    int hearts;
    int diamonds;
    int clubs;
    int spades;
};
struct Computer{
    Cards hand;
    int player;
};
struct User{
    string name;
    string password;
    int money;
    int times_played;
    int high_score;
};


//ADREÇA DEL FITXER
const string ADDRESS = "C:\\Users\\Nil\\Desktop\\Prog\\Casino\\dades.txt";

//EXPLANATIONS
const string roulette_explanation = "You can bet on a number (from 1 to 36) or on even/odd.\nIf zero comes out, you lose no matter what.\n";
const string blackjack_explanation = "Your goal is to end the game with more points than the Dealer.\nAt the start of the game, you and the dealer are both dealt two cards, but you can only see one of his cards.\nYou can end the game at any moment, but you must have at least 10 points to win.\nIf the game is not stopped earlier, whoever goes above 21 points first loses.\nIn the event of a tie, the dealer wins.\n";
const string poker_explanation = "Poker is played against 3 computer-controlled players.\nEvery player is dealt 2 cards.\nThen you can bet, and 3 more are revealed on the table (the flop).\nThen you can bet again, and 1 more is revealed on the table (the turn).\nThen you can bet again, and 1 more is revealed on the table (the river).\nYou can now bet for the last time and the hands of those players still standing are compared.\nThe ranking of hands goes as follows:\nStraight flush > Quads > Full house > Flush > Straight > Threes > Two pair > Pair > Highest card\n";

//MESSAGES
const string error_options_1_2 = "Whoops. Please enter either 1 or 2";
const string options_menu = "Press 0 to exit.\nPress 1 to play Roulette.\nPress 2 to play Blackjack.\nPress 3 to play Poker.\nPress 4 to see the high scores.\nPress 5 to read the rules for all the games.\nPress 6 to change your password.\n";
const string error_options_menu = "Whoops. Enter a number between 0 and 6. ";
const string options_roulette = "Press 1 to bet on a number.\nPress 2 to bet on even/odd.\n";
const string message_bet = "How much do you want to bet? ";
const string error_bet = "Whoops. Enter a number between 1 and your current money. ";
const string error_bet_poker = "Whoops. Incorrect amount.";
const string guess_roulette = "Choose a number between 1 and 36 to bet on: ";
const string error_guess_roulette = "Whoops. Enter a number between 1 and 36. ";
const string guess_even_odd = "Press 1 to bet on 'odd'.\nPress 2 to bet on 'even'\n";
const string options_blackjack = "Press 1 to receive a card.\nPress 2 to stand.\n";
const string blackjack_ace_choice = "Press 1 if you want this ace to be worth 1 point.\nPress 2 if you want this ace to be worth 11 points.\n";
const string choice_poker = "Press 1 to continue playing.\nPress 2 to fold now.\n";
const string choice_allin = "Press 1 to go all in.\nPress 2 to fold now.\n";

int get_int(int minimum, int maximum, string query, string err){
    int Quantity;
    while ((cout << query and !(cin >> Quantity))  or (Quantity < minimum) or (Quantity > maximum) or cin.peek() != '\n'){    
        cout << err << endl;    
        cin.clear();
        cin.ignore();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return Quantity;
}

int get_random_int(int minimum, int maximum){
    return rand()%(maximum-minimum + 1) + minimum;
}

Deck create_deck(int n){
    Deck D(n, false);
    return D;
}

string encrypt(string s){
    string encrypt = s;
    int size = s.size();
    for (int i = 0; i < size; i++){
        if (i%2 == 0) encrypt[i] += i;
        else encrypt[i] -= i;
    }
    return encrypt;
}

string decode(int n){
    //hearts, diamonds: red
    //clubs, spades: black
    string s = "";
    if (n%13 == 0) s += "A";
    else if (n%13 < 9) s += char('0' + n%13 + 1);
    else if (n%13 == 9) s += "10";
    else if (n%13 == 10) s += "J";
    else if (n%13 == 11) s += "Q";
    else s += "K";
    if (n < 13) s += " of hearts";
    else if (n < 26) s += " of diamonds";
    else if (n < 39) s += " of clubs";
    else s += " of spades";
    return s;
}

int max(const vector<int>& v){
    int max = v[0];
    int size = v.size();
    for (int i = 1; i < size; i++){
        if (v[i] > max) max = v[i];
    }
    return max;
}

int check_for_consecutive(const Cards& hand, int min, int max){
    int size = hand.size();
    Cards cards(14, 0);
    for (int i = 0; i < size; i++){
        if (hand[i] >= min and hand[i] <= max){
            if (hand[i]%13 == 0) cards[0]++, cards[12]++;
            else cards[hand[i]%13]++;
        }
    }
    for (int i = 10; i >= 0; i--){
        bool good = true;
        for (int j = 0; j < 5; j++){
            if (cards[i+j] != 1) good = false; 
        }
        if (good) return i+4;
    }
    return -1;
}

Hand create_hand(const Cards& hand, const Cards& table){
    Cards total(2);
    for (int i = 0; i < 2; i++) total[i] = hand[i];
    for (int i = 0; i < 5 and table[i] != -1; i++) total.push_back(table[i]);
    Hand total_hand;
    total_hand.cards = Cards(13, 0);
    total_hand.royal_flush = -1;
    total_hand.hearts = total_hand.diamonds = total_hand.clubs = total_hand.spades = 0;
    for (int i = 0; i < int(total.size()); i++){
        if (total[i]%13 == 0) total_hand.cards[12]++;
        else total_hand.cards[total[i]%13 - 1]++;
        if (total[i] < 13) total_hand.hearts++;
        else if (total[i] < 26) total_hand.diamonds++;
        else if (total[i] < 39) total_hand.clubs++;
        else total_hand.spades++;
    }
    bool hearts = total_hand.hearts >= 5;
    bool diamonds = total_hand.diamonds >= 5;
    bool clubs = total_hand.clubs >= 5;
    bool spades = total_hand.spades >= 5;
    
    if (not hearts and not diamonds and not clubs and not spades) total_hand.royal_flush = -1;
    else{
        int min, max;
        if (hearts) min = 0, max = 12;
        if (diamonds) min = 13, max = 25;
        if (clubs) min = 26, max = 38;
        if (spades) min = 39, max = 51;
        total_hand.royal_flush = check_for_consecutive(total, min, max);
    }
    return total_hand;
}

//1 means A wins
//2 means B wins
//0 means not decided yet (no poker)
int check_quads(const Hand& a, const Hand& b){
    bool tie = false;
    for (int i = 12; i >= 0 and not tie; i--){
        if (a.cards[i] == 4 and b.cards[i] == 4) tie = true;
        else if (a.cards[i] == 4) return 1;
        else if (b.cards[i] == 4) return 2;
    }
    if (not tie) return 0;
    for (int i = 12; i >= 0; i--){
        if (a.cards[i] == 1) return 1;
        if (b.cards[i] == 1) return 2;
    }
    return 0;
}

//1 means A wins
//2 means B wins
//0 means not decided yet (no full)
int check_full(const Hand& a, const Hand& b){
    int three_a = -1;
    int three_b = -1;
    int two_a = -1;
    int two_b = -1;
    for (int i = 12; i >= 0; i--){
        if (a.cards[i] == 3) three_a = i;
        if (b.cards[i] == 3) three_a = i;
        if (a.cards[i] == 2) two_a = i;
        if (b.cards[i] == 2) two_b = i;
    }
    bool full_a = (three_a != -1 and two_a != -1);
    bool full_b = (three_b != -1 and two_b != -1);
    if (full_a and full_b){
        if (three_a > three_b) return 1;
        if (three_b > three_a) return 2;
        if (two_a >= two_b) return 1;
        return 2;
    }
    if (full_a) return 1;
    if (full_b) return 2;
    return 0;
}

//if there's a flush, returns value of highest card
//else returns -1
int check_flush(const Hand& a){
    bool flush = (a.hearts >= 5 or a.diamonds >= 5 or a.clubs >= 5 or a.spades >= 5);
    if (flush){
        for (int i = 12; i >= 0; i--){
            if (a.cards[i] != 0) return i;
        }
    }
    return -1;
}

//if there's a straight, returns value of highest card
//else returns -1
int check_straight(const Hand& a){
    for (int i = 0; i < 9; i++){
        if (a.cards[i] >= 1){
            if (a.cards[i+1] >= 1 and a.cards[i+2] >= 1 and a.cards[i+3] >= 1 and a.cards[i+4] >= 1) return i+4;
            return -1;
        }
    }
    if (a.cards[12] >= 1 and a.cards[0] >= 1 and a.cards[1] >= 1 and a.cards[2] >= 1 and a.cards[3] >= 1){
        return 3;
    }
    return -1;
}

//returns 1 if A wins and 2 if B wins
//returns 0 if there's not a three
int check_three(const Hand& a, const Hand& b){
    bool tie = false;
    for (int i = 12; i >= 0 and not tie; i--){
        if (a.cards[i] == 3 and b.cards[i] == 3) tie = true;
        else if (a.cards[i] == 3) return 1;
        else if (b.cards[i] == 3) return 2;
    }
    if (not tie) return 0;
    for (int i = 12; i >= 0; i--){
        if (a.cards[i] == 1 and b.cards[i] != 1) return 1;
        if (b.cards[i] == 1 and a.cards[i] != 1) return 2;
    }
    return 1;
}

//1 means A wins
//2 means B wins
//0 means not decided yet (no two pair)
int check_two_pair(const Hand& a, const Hand& b){
    int two_a = -1;
    int two_b = -1;
    int two2_a = -1;
    int two2_b = -1;
    for (int i = 12; i >= 0; i--){
        if (a.cards[i] == 2 and two_a == -1) two_a = i;
        else if (a.cards[i] == 2) two2_a = i;
        if (b.cards[i] == 2 and two_b == -1) two_b = i;
        else if (b.cards[i] == 2) two2_b = i;
    }
    bool twopair_a = (two_a != -1 and two2_a != -1);
    bool twopair_b = (two_b != -1 and two2_b != -1);
    if (not twopair_a and not twopair_b) return 0;
    if (twopair_a and twopair_b){
        if (two_a > two_b) return 1;
        if (two_b > two_a) return 2;
        if (two2_a > two2_b) return 1;
        if (two2_b > two2_a) return 2;
        for (int i = 12; i >= 0; i--){
            if (a.cards[i] == 1) return 1;
            if (b.cards[i] == 1) return 2;
        }
    }
    if (twopair_a) return 1;
    return 2;
}

//returns 1 if A wins and 2 if B wins
//returns 0 if there's not a pair
int check_pair(const Hand& a, const Hand& b){
    bool tie = false;
    for (int i = 12; i >= 0 and not tie; i--){
        if (a.cards[i] == 2 and b.cards[i] == 2) tie = true;
        else if (a.cards[i] == 2) return 1;
        else if (b.cards[i] == 2) return 2;
    }
    if (not tie) return 0;
    for (int i = 12; i >= 0; i--){
        if (a.cards[i] == 1 and b.cards[i] != 1) return 1;
        if (b.cards[i] == 1 and a.cards[i] != 1) return 2;
    }
    return 1;
}

//returns 1 if A wins or there's a tie
//returns 2 otherwise
int check_highest(const Hand& a, const Hand& b){
    for (int i = 12; i >= 0; i--){
        if (a.cards[i] != 0 and b.cards[i] == 0) return 1;
        if (b.cards[i] != 0 and a.cards[i] == 0) return 2;
    }
    return 1;
}

//true means A wins
//false means B wins
//in case of a tie, player A wins (random process)
bool poker_winner(const Hand& a, const Hand& b){
    //royal flush
    if (a.royal_flush != b.royal_flush) return (a.royal_flush > b.royal_flush);
    if (a.royal_flush != -1 and b.royal_flush != -1) return true;
    
    //quads
    int quads = check_quads(a, b);
    if (quads == 1) return true;
    if (quads == 2) return false;
    
    //full house
    int full = check_full(a, b);
    if (full == 1) return true;
    if (full == 2) return false;
    
    //flush
    int flush_a = check_flush(a);
    int flush_b = check_flush(b);
    if (flush_a > flush_b) return true;
    if (flush_b > flush_a) return false;
    if (flush_a != -1 and flush_b != -1) return true;
    
    //straight
    int straight_a = check_straight(a);
    int straight_b = check_straight(b);
    if (straight_a != straight_b) return (straight_a > straight_b);
    if (straight_a != -1 and straight_b != -1) return true;

    //three
    int three = check_three(a, b);
    if (three == 1) return true;
    if (three == 2) return false;

    //two pair
    int two_pair = check_two_pair(a, b);
    if (two_pair == 1) return true;
    if (two_pair == 2) return false;
    
    //pair
    int pair = check_pair(a, b);
    if (pair == 1) return true;
    if (pair == 2) return false;
    
    //highest card
    int high = check_highest(a, b);
    if (high == 1) return true;
    return false;
}

int blackjack_points(int card){
    if (card%13 == 0){
        int choice = get_int(1, 2, blackjack_ace_choice, error_options_1_2);
        if (choice == 1) return 1;
        return 11;
    }
    if (card%13 < 9) return card%13+1;
    return 10;
}

int draw_card(Deck& D){
    int size = D.size();
    bool correct = false;
    int r;
    while (not correct){
        r = get_random_int(0, size-1);
        if (not D[r]) correct = true;
    }
    D[r] = true;
    return r;
}

void roulette(int& money){
    cout << endl << "Welcome to the Roulette!" << endl;
    cout << "You have " << money << " coins." << endl;
    int bet = get_int(1, money, message_bet, error_bet);
    money -= bet;
    int choice = get_int(1, 2, options_roulette, error_options_1_2);
    int winner = get_random_int(0, 36);
    if (choice == 1){
        int guess = get_int(1, 36, guess_roulette, error_guess_roulette);
        cout << endl << "The winner is " << winner << "." << endl;
        if (guess == winner){
            cout << "You won " << 35*bet << " coins!" << endl;
            money += 36*bet;
        }
        else{
            cout << "You lost..." << endl;
        }
    }
    else{
        int guess = get_int(1, 2, guess_even_odd, error_options_1_2);
        cout << endl << "The winner is " << winner << "." << endl;
        if (guess%2 == winner%2 and winner != 0){
            cout << "You won " << bet << " coins!" << endl;
            money += 2*bet;
        }
        else{
            cout << "You lost..." << endl;
        }
    }
}

int decide_ace(int score){
    if (score <= 10) return 11;
    return 1;
}

int rate_cards(Cards& hand, Cards& table, string& s){
    Hand total = create_hand(hand, table);
    
    int max = 0;
    int bestcard = 0;
    for (int i = 12; i >= 0; i--){
        if (total.cards[i] > max){
            max = total.cards[i];
            bestcard = i;
        } 
    }
    s = "royal flush";
    if (total.royal_flush != -1) return 100-(12-total.royal_flush)/4;
    s = "quads";
    if (max == 4) return 95-(12-bestcard)/4;
    s = "full";
    if (max == 3){
        for (int i = 12; i >= 0; i--){
            if (total.cards[i] == 2) return 85 - (12-bestcard)/4 - (12-i)/4;
        }
    }
    s = "flush";
    int flush = check_flush(total);
    if (flush != -1) return 75 - (12-flush)/4;
    s = "straight";
    int straight = check_straight(total);
    if (straight != -1) return 70 - (12-straight)/4;
    s = "three";
    if (max == 3) return 60 - (12-bestcard)/2;
    s = "two pair";
    if (max == 2){
        for(int i = 0; i < 13; i++){
            if (total.cards[i] == 2 and i != bestcard) return 50-(bestcard-12)/4-(i-12)/4;
        }
    }
    s = "pair";
    if (max == 2) return 40 - (12-bestcard);
    s = "high card";
    return 2*bestcard;
}

void blackjack(int& money){
    cout << endl << "Welcome to Blackjack!" << endl;
    cout << "You have " << money << " coins." << endl;
    int bet = get_int(1, money, message_bet, error_bet);
    money -= bet;
    Deck D = create_deck(52);
    bool cont = true;
    int score = 0;
    int dealer_score = 0;
        
    //initial hands: 2 cards face up for you, 1 card face up for the dealer
    int initial_card = draw_card(D);
    cout << endl << "You got a " << decode(initial_card) << endl;
    score += blackjack_points(initial_card);
    initial_card = draw_card(D);
    cout << "You got a " << decode(initial_card) << endl;
    score += blackjack_points(initial_card);
    initial_card = draw_card(D);
    
    cout << endl << "The dealer got a " << decode(initial_card) << endl;
    if (initial_card%13 == 0) dealer_score += decide_ace(dealer_score);
    else dealer_score += blackjack_points(initial_card);
    int dealer_tapped_card = draw_card(D);
    
    cout << "The dealer's visible score is: " << dealer_score << endl << endl;

    if (dealer_tapped_card%13 == 0) dealer_score += decide_ace(dealer_score);
    else dealer_score += blackjack_points(dealer_tapped_card);    

    while (cont and score < 21 and dealer_score < 21){
        cout << "Your score is: " << score << endl;
        int choice = get_int(1, 2, options_blackjack, error_options_1_2);
        if (choice == 2){
            cout << "The dealer's tapped card was a " << decode(dealer_tapped_card) << endl;
            cont = false;
        }
        else if (choice == 1){
            int card = draw_card(D);
            cout << endl << "You got a " << decode(card) << endl;
            score += blackjack_points(card);
            if (score == 21){
                cont = false;
            }
            else if (score < 21){
                if (dealer_score < 16 or get_random_int(0, 1) == 1){
                    card = draw_card(D);
                    if (card%13 == 0) dealer_score += decide_ace(dealer_score);
                    else dealer_score += blackjack_points(card);
                    if (dealer_score == 21){
                        cont = false;
                    }
                }
                else cout << "The dealer has skipped his turn." << endl;
            }
        }
    }
    if (score == 21) cout << "Blackjack!" << endl;
    else if (dealer_score == 21) cout << "Dealer's Blackjack" << endl;
    cout << "Your score is: " << score << endl;
    cout << "The dealer's score is: " << dealer_score << endl << endl;
    bool won;
    if (score > 21){
        cout << "You busted." << endl;
        won = false;
    }
    else if (dealer_score > 21){
        cout << "The dealer busted." << endl;
        won = true;
    }
    else if (score < 10) won = false;
    else won = (score > dealer_score);
    if (won){
        cout << "You won " << bet << " coins!" << endl;
        money += 2*bet;
    }    
    else{
        cout << endl << "You lost..." << endl;   
    }
}

int compute_bet(Computer& pc, vector<int>& bets, vector<bool>& ingame, Cards& table, int counter){
    int call = max(bets)-bets[pc.player];
    //TEMPORARY:
    //if (get_random_int(0, 100) > 95) return 2*(10+call);
    //return call;
    //END TEMPORARY
    
    
    int i = 0;
    while (table[i] != -1 and i < 5) i++;
    
    //decides what to do
    int r = get_random_int(0, 100);
    if (i == 0) r /= 4;
    if (i == 3) r -= r/4;
    if (i == 4) r -= r/8;
    
    //feeling lucky
    if (r > 80) return (call + get_random_int(0, 3)*(call/2));
    
    string s; //useless, but we need an argument for rate
    int rate = rate_cards(pc.hand, table, s);
    
    if (counter == 2){
        //fold
        if (r > rate*3){
            if (max(bets) == 0) return 0;
            return -1;
        }
        //call
        return max(bets);
    }
    else{
        //raise
        if (rate > 90) return (10+call)*get_random_int(1, 4);
        if (rate > 80) return (10+call)*get_random_int(1, 3);
        if (rate > 65) return (10+call)*get_random_int(1, 2);
        if (rate > 50 and get_random_int(0, 100) > 5) return call + call/2;
        //fold
        if (r > rate*3){
            if (call == 0) return 0;
            return -1;
        }
        //call
        else return call;
    }
}
//TODO
bool stays(Computer& pc, vector<int>& bets, vector<bool>& ingame){
    return true;
}

//"negotiation" to bet
void negotiate(int& money, bool& fold, bool& all_in, bool& primer, vector<int>& bets, vector<bool>& ingame, Computer& pc1, Computer& pc2, Computer& pc3, Cards& table){
    bool agree = false;
    int counter = 0;
    while (not agree and counter < 3){
        int minimum = max(bets)-bets[0];

        if (primer){
            primer = false;
            minimum = 5;
        }
        else{
            cout << "The current maximum bet is " << max(bets) << endl;
            cout << "Your current bet is " << bets[0] << endl;
            int choice = get_int(1, 2, choice_poker, error_options_1_2);
            if (choice == 2){
                fold = true;
                cout << "You folded!" << endl;
                cout << "Returning to the casino..." << endl;
                return;
            }
        }

        if (money < minimum){
            cout << "You have less money than the minimum bet. Wanna go all in?" << endl;
            int choice = get_int(1, 2, choice_allin, error_options_1_2);
            if (choice == 2){
                fold = true;
                cout << "You folded!" << endl;
                cout << "Returning to the casino..." << endl;
                return;
            }
            else{
                all_in = true;
                bets[0] += money;
                if (ingame[1]){
                    if (stays(pc1, bets, ingame)){
                        bets[1] = bets[0];
                        cout << "Computer 1 has matched your all-in!" << endl;
                    }
                    else{
                        ingame[1] = false;
                        cout << "Computer 1 has folded!" << endl;
                    }
                }
                if (ingame[2]){
                    if (stays(pc1, bets, ingame)){
                        bets[2] = bets[0];
                        cout << "Computer 2 has matched your all-in!" << endl;
                    }
                    else{
                        ingame[2] = false;
                        cout << "Computer 2 has folded!" << endl;
                    }
                }
                if (ingame[3]){
                    if (stays(pc1, bets, ingame)){
                        bets[3] = bets[0];
                        cout << "Computer 3 has matched your all-in!" << endl;
                    }
                    else{
                        ingame[3] = false;
                        cout << "Computer 3 has folded!" << endl;
                    }
                }
                money = 0;
                return;
            }
        }
        else{
            int maximum = money;
            if (counter == 2) maximum = max(bets)-bets[0];
            cout << "You have " << money << " coins." << endl;
            int bet = get_int(minimum, maximum, message_bet, error_bet_poker);
            money -= bet;
            bets[0] += bet;
            
            cout << endl;
            
            if (ingame[1]){
                int pc1_bet = compute_bet(pc1, bets, ingame, table, counter);
                if (pc1_bet == -1){
                    cout << "Computer 1 has folded!" << endl;
                    ingame[1] = false;
                }
                else bets[1] += pc1_bet;
            }
            
            if (ingame[2]){
                int pc2_bet = compute_bet(pc2, bets, ingame, table, counter);
                if (pc2_bet == -1){
                    cout << "Computer 2 has folded!" << endl;
                    ingame[2] = false;
                }
                else bets[2] += pc2_bet;
            }
        
            if (ingame[3]){
                int pc3_bet = compute_bet(pc3, bets, ingame, table, counter);
                if (pc3_bet == -1){
                    cout << "Computer 3 has folded!" << endl;
                    ingame[3] = false;
                }
                else bets[3] += pc3_bet;
            }

            agree = true;
            for (int i = 1; i < 4; i++){
                if (ingame[i] and bets[i] != bets[0]) agree = false;
            }
            cout << "The maximum bet has been " << max(bets) << endl;
        }
        counter++;
    }
}

void poker(int& money){
    cout << endl << "Welcome to Poker!" << endl;
    cout << "You have " << money << " coins." << endl << endl;
    
    Deck D = create_deck(52);
    Computer pc1, pc2, pc3;
    pc1.player = 1;
    pc2.player = 2;
    pc3.player = 3;
    pc1.hand = pc2.hand = pc3.hand = Cards(2, -1);
    Cards table(5, -1);
    
    //here is where all bets go
    int cumulative = 0;
    //player gets initial cards
    Cards your_hand(2);
    your_hand[0] = draw_card(D);
    cout << "You got a " << decode(your_hand[0]) << endl;
    your_hand[1] = draw_card(D);
    cout << "You got a " << decode(your_hand[1]) << endl << endl;
    //computer gets initial cards
    for (int i = 0; i < 2; i++){
        pc1.hand[i] = draw_card(D);
        pc2.hand[i] = draw_card(D);
        pc3.hand[i] = draw_card(D);
    }
    
    //bets[0] is player, bets[i] = pc(i)_bet for i = 1,2,3
    vector<bool> ingame(4, true);
    vector<int> bets(4, 0);
    bool all_in = false;
    bool primer = true;
    bool fold = false;
    //bets are made
    negotiate(money, fold, all_in, primer, bets, ingame, pc1, pc2, pc3, table);
    //bets are added to the total and reset
    cumulative += bets[0] + bets[1] + bets[2] + bets[3];
    cout << "CUMULATIVE POT: " << cumulative << endl;
    bets[0] = bets[1] = bets[2] = bets[3] = 0;
    if (fold) return;
    //if every opponent is out
    if (ingame[1] == false and ingame[2] == false and ingame[3] == false){
        cout << "Every opponent folded, you won!" << endl;
        cout << "You got " << cumulative << " coins!" << endl;
        money += cumulative;
        return;
    }
    
    cout << endl << "THE FOLD" << endl;
    for (int i = 0; i < 3; i++){
        table[i] = draw_card(D);
        cout << decode(table[i]) << endl;
    }
    cout << endl;
    
    if (not all_in) negotiate(money, fold, all_in, primer, bets, ingame, pc1, pc2, pc3, table);
    //bets are added to the total and reset
    cumulative += bets[0] + bets[1] + bets[2] + bets[3];
    cout << "CUMULATIVE POT: " << cumulative << endl;
    bets[0] = bets[1] = bets[2] = bets[3] = 0;
    if (fold) return;
    //if every opponent is out
    if (ingame[1] == false and ingame[2] == false and ingame[3] == false){
        cout << "Every opponent folded, you won!" << endl;
        cout << "You got " << cumulative << " coins!" << endl;
        money += cumulative;
        return;
    }
    
    cout << endl << "THE TURN" << endl;
    table[3] = draw_card(D);
    cout << decode(table[3]) << endl << endl;
    
    if (not all_in) negotiate(money, fold, all_in, primer, bets, ingame, pc1, pc2, pc3, table);
    //bets are added to the total and reset
    cumulative += bets[0] + bets[1] + bets[2] + bets[3];
    cout << "CUMULATIVE POT: " << cumulative << endl;
    bets[0] = bets[1] = bets[2] = bets[3] = 0;
    if (fold) return;
    //if every opponent is out
    if (ingame[1] == false and ingame[2] == false and ingame[3] == false){
        cout << "Every opponent folded, you won!" << endl;
        cout << "You got " << cumulative << " coins!" << endl;
        money += cumulative;
        return;
    }
    
    cout << endl << "THE RIVER" << endl;
    table[4] = draw_card(D);
    cout << decode(table[4]) << endl << endl;
    
    if (not all_in) negotiate(money, fold, all_in, primer, bets, ingame, pc1, pc2, pc3, table);
    //bets are added to the total and reset
    cumulative += bets[0] + bets[1] + bets[2] + bets[3];
    cout << "CUMULATIVE POT: " << cumulative << endl;
    if (fold) return;
    //if every opponent is out
    if (ingame[1] == false and ingame[2] == false and ingame[3] == false){
        cout << "Every opponent folded, you won!" << endl;
        cout << "You got " << cumulative << " coins!" << endl;
        money += cumulative;
        return;
    }
    
    vector<Hand> Hands(4);
    Hands[0] = create_hand(your_hand, table);
    Hands[1] = create_hand(pc1.hand, table); 
    Hands[2] = create_hand(pc2.hand, table); 
    Hands[3] = create_hand(pc3.hand, table); 
    for (int i = 0; i < 4; i++) Hands[i].player = i;
    
    sort(Hands.begin(), Hands.end(), poker_winner);
    int winner = -1;
    vector<string> s(4);
    rate_cards(your_hand, table, s[0]);
    rate_cards(pc1.hand, table, s[1]);
    rate_cards(pc2.hand, table, s[2]);
    rate_cards(pc3.hand, table, s[3]);
    //for (int i = 0; i < 4; i++){
        //cout << i+1 << " PLACE: " << Hands[i].player << " with a " << s[Hands[i].player] << endl;
        //cout << "Hearts: " << Hands[i].hearts << endl;
        //cout << "Diamonds: " << Hands[i].diamonds << endl;
        //cout << "Clubs: " << Hands[i].clubs << endl;
        //cout << "Spades: " << Hands[i].spades << endl;
    //}
    for (int i = 0; i < 4 and winner == -1; i++){
        if (ingame[Hands[i].player]) winner = Hands[i].player;
    }
    
    cout << endl << "TABLE:" << endl;
    for (int i = 0; i < 5; i++) cout << decode(table[i]) << endl;
    cout << endl << "YOUR HAND:" << endl;
    cout << decode(your_hand[0]) << endl << decode(your_hand[1]) << endl;
    cout << endl << "COMPUTER 1:" << endl;
    if (not ingame[1]) cout << "had folded" << endl;
    else cout << decode(pc1.hand[0]) << endl << decode(pc1.hand[1]) << endl;
    cout << endl << "COMPUTER 2:" << endl;
    if (not ingame[2]) cout << "had folded" << endl;
    else cout << decode(pc2.hand[0]) << endl << decode(pc2.hand[1]) << endl;
    cout << endl << "COMPUTER 3:" << endl;
    if (not ingame[3]) cout << "had folded" << endl;
    else cout << decode(pc3.hand[0]) << endl << decode(pc3.hand[1]) << endl;
    
    cout << endl;
    if (winner == 0){
        cout << "You won!" << endl;
        cout << "You got " << cumulative << " coins!" << endl;
        money += cumulative;
    }
    else cout << "The winner was Computer " << winner << endl << "You lost..." << endl;
    return;
    
}

void overwrite(vector<User>& Users, int money, string name){
    ofstream file;
    file.open(ADDRESS, ios::trunc);
    file.close();
    file.open(ADDRESS);
    for (int i = 0; i < int(Users.size()); i++){
        if (Users[i].money == 0){
            Users[i].money = 50;
            Users[i].times_played = 0;
        }
        if (Users[i].name == name){
            Users[i].money = money;
            if (money > Users[i].high_score){
                Users[i].high_score = money;
            }
        }
        file << Users[i].name << " " << Users[i].password << " " << Users[i].money << " " << Users[i].high_score << " " << Users[i].times_played << endl;
    }
    file.close();
}

bool comp(const User& u1, const User& u2){
    return (u1.high_score > u2.high_score);
}

void show_highscores(const vector<User>& Users){
    vector<User> copy = Users;
    sort(copy.begin(), copy.end(), comp);
    cout << endl << "HIGH SCORES:" << endl;
    int max = 0;
    for (int i = 0; i < int(copy.size()); i++){
        if (int(copy[i].name.size()) > max) max = int(copy[i].name.size());
    }
    for (int i = 0; i < int(copy.size()) and i < 10; i++){
        int l = copy[i].name.size();
        cout << i+1 << ". " << copy[i].name << " ";
        for (int i = 0; i < max+2-l; i++) cout << ".";
        cout << " " << copy[i].high_score << endl;
    }
}

void change_password(vector<User>& Users, int index, bool& exit){
    int count = 0;
    bool correct = false;
    while (count < 3 and not correct){
        cout << endl << "Enter your password: ";
        string pass;
        cin >> pass;
        if (encrypt(pass) == Users[index].password) correct = true;
        else cout << "Wrong password." << endl;
        count++;
    }
    if (not correct){
        cout << "Too many attempts. Exiting..." << endl;
        exit = true;
        return;
    }
    correct = false;
    string password1;
    while (not correct){
        string password2;
        cout << "Enter your new password: ";
        cin >> password1;
        cout << "Re-enter your new password: ";
        cin >> password2;
        if (password1 == password2) correct = true;
        else cout << "The passwords do not match." << endl;
        if (int(password1.size()) < 5){
            cout << "The password is too short." << endl;
            correct = false;
        }
    }
    Users[index].password = encrypt(password1);
    cout << "Password changed successfully." << endl;
}

int main(){
    srand(time(NULL));
    bool end = false;
    
    vector<User> Users;
	ifstream file;
    file.open(ADDRESS, ios::in);
    if (!file) {
        cout << "Unable to open file dades.txt" << endl;
        return 1;   // call system to stop
    }
    User u;
    while (file >> u.name >> u.password >> u.money >> u.high_score >> u.times_played){
        Users.push_back(u);
    }
    file.close();
            
    int previous_user = -1;
    int money = 50;
    cout << "Welcome to the Casino!" << endl;
    cout << "What's your name? ";
    string name;
    cin >> name;
        
    for (int i = 0; i < int(Users.size()) and previous_user == -1; i++){
        if (Users[i].name == name){
            previous_user = i;
            money = Users[i].money;
        }
    }
    
    if (previous_user != -1){ 
        int count = 0;
        bool correct = false;
        while (count < 3 and not correct){
            cout << endl << "Enter your password: ";
            string pass;
            cin >> pass;
            if (encrypt(pass) == Users[previous_user].password) correct = true;
            else cout << "Wrong password." << endl;
            count++;
        }
        if (not correct){
            cout << "Too many attempts. Exiting..." << endl;
            return 1;
        }
        cout << "Hi again, " << name << "!" << endl;
        Users[previous_user].times_played++;
        if (Users[previous_user].money <= 0){
            Users[previous_user].money = 50;
            Users[previous_user].times_played = 1;
            money = 50;
        }
    }
    
    else{
        cout << "Welcome, " << name << "!" << endl;
        bool correct = false;
        string password1;
        while (not correct){
            string password2;
            cout << "Enter a password: ";
            cin >> password1;
            cout << "Re-enter the password: ";
            cin >> password2;
            if (password1 == password2) correct = true;
            else cout << "The passwords do not match." << endl;
            if (int(password1.size()) < 5){
                cout << "The password is too short." << endl;
                correct = false;
            }
        }
        User new_user;
        new_user.name = name;
        new_user.password = encrypt(password1);
        new_user.money = 50;
        new_user.high_score = 50;
        new_user.times_played = 1;
        Users.push_back(new_user);
        previous_user = Users.size() -1;
    }

    //GAME LOOP
    while (not end and money > 0){
        overwrite(Users, money, name);
        cout << endl << endl << "You have " << money << " coins." << endl;
        int choice = get_int(0, 6, options_menu, error_options_menu);
        if (choice == 0) end = true;
        else if (choice == 1) roulette(money);
        else if (choice == 2) blackjack(money);
        else if (choice == 3){
            if (money > 5) poker(money);
            else cout << "Sorry, you must have at least 5 coins to enter a poker game";
        }
        else if (choice == 4){
            show_highscores(Users);
        }
        else if (choice == 5){
            cout << endl << "ROULETTE:" << endl;
            cout << roulette_explanation << endl;
            cout << "BLACKJACK:" << endl;
            cout << blackjack_explanation << endl;
            cout << "POKER:" << endl;
            cout << poker_explanation;
        }
        else{
            bool exit = false;
            change_password(Users, previous_user, exit);
            if (exit) return 1;
        }
    }
    
    if (money == 0){
        cout << endl << "You're broke! Game over." << endl;
        cout << "You had played " << Users[previous_user].times_played << " times." << endl;
        cout << "Your high score is " << Users[previous_user].high_score << endl;
    }
    else cout << endl << "You have " << money << " coins. See you soon!" << endl;
    overwrite(Users, money, name);
    file.close();
}
