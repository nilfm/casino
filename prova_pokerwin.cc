#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
#include <windows.h>
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

int get_random_int(int minimum, int maximum){
    return rand()%(maximum-minimum + 1) + minimum;
}

Deck create_deck(int n){
    Deck D(n, false);
    return D;
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

int check_for_consecutive(const Cards& hand, int min, int max){
    int size = hand.size();
    Cards cards(14, 0);
    for (int i = 0; i < size; i++){
        if (hand[i] >= min and hand[i] <= max){
            if (hand[i]%13 == 0) cards[0]++, cards[13]++;
            else cards[hand[i]%13]++;
        }
    }
    for (int i = 10; i >= 0; i--){
        bool good = true;
        for (int j = 0; j < 5; j++){
            if (cards[i+j] < 1) good = false; 
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
        if (b.cards[i] == 3) three_b = i;
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
        else if (a.cards[i] == 3){
            return 1;
        }
        else if (b.cards[i] == 3){
            return 2;
        }
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

int main(){
    srand(time(NULL));
    string s;
    bool careful = true;
    while (not careful or cin >> s){
        careful = false;
        Deck D = create_deck(52);
        Cards table(5);
        for (int i = 0; i < 5; i++){
            table[i] = draw_card(D);
        }
        Cards hand1(2);
        for (int i = 0; i < 2; i++){
            hand1[i] = draw_card(D);
        }
        Cards hand2(2);
        for (int i = 0; i < 2; i++){
            hand2[i] = draw_card(D);
        }
        Hand h1 = create_hand(hand1, table);
        Hand h2 = create_hand(hand2, table);
        bool win = poker_winner(h1, h2);
        string s1, s2;
        int r1 = rate_cards(hand1, table, s1);
        int r2 = rate_cards(hand2, table, s2);
        //if (s1 == "straight" or s2 == "straight") careful = true; //set conditions for when to stop
        careful = true; //stop at every iteration
        if (careful){
            cout << "TABLE" << endl;
            for (int i = 0; i < 5; i++){
                cout << decode(table[i]) << endl;
            }
            cout << endl;
            cout << "HAND 1" << endl;
            for (int i = 0; i < 2; i++){
                cout << decode(hand1[i]) << endl;
            }
            cout << endl;
            cout << "HAND 2" << endl;
            for (int i = 0; i < 2; i++){
                cout << decode(hand2[i]) << endl;
            }
            cout << endl;
            cout << "Player 1 had a " << s1 << ". Score: " << r1 << endl;
            cout << "Player 2 had a " << s2 << ". Score: " << r2 << endl;
            if (win) cout << "PLAYER 1 WINS" << endl;
            else cout << "PLAYER 2 WINS" << endl;
            cout << endl << endl;
        } 
    }
}
