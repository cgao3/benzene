//----------------------------------------------------------------------------
/** @file BenzenePlayer.hpp
 */
//----------------------------------------------------------------------------

#ifndef BENZENEPLAYER_HPP
#define BENZENEPLAYER_HPP

#include "HexBoard.hpp"
#include "HexEval.hpp"
#include "HexPlayer.hpp"
#include "ICEngine.hpp"

_BEGIN_BENZENE_NAMESPACE_

//----------------------------------------------------------------------------

/** Abstract base class for all UofA players. */
class BenzenePlayer: public HexPlayer
{
public:
    explicit BenzenePlayer();

    virtual ~BenzenePlayer();

    /** Generates a move from this board position. If the game is
        already over (somebody has won), returns RESIGN.

        Derived UofA players that use different search algorithms
        should not extend this method, but the protected virtual
        method search() below.

        Classes deriving from UofAFunctinality should extend the 
        pre_search() method only.  
        @see BenzenePlayerFunctionality.

        This method does the following:

        1 - if state is terminal (game over, vc/fill-in win/loss),
            returns "appropriate" move. Otherwise, continues to 
            step 2. 
        2 - Calls pre_search().
            If pre_search() returns INVALID_POINT, continues to 
            step 3. Otherwise, returns point returned by pre_search().
        3 - returns move returned by search() 

        @param brd HexBoard to do work on. Board position is set to
               the board position as that of the game board. 
        @param game_state Game history up to this position.
        @param color Color to move in this position.
        @param max_time Time in minutes remaining in game.
        @param score Return score of move here. 
    */
    HexPoint genmove(HexBoard& brd, const Game& game_state, HexColor color,
                     double max_time, double& score);

    //----------------------------------------------------------------------

    /** Performs various checks before the actual search. An example
        usage of this method would be to check an opening book for the
        current state and to abort the call to search() if found.
        
        If pre_search() is successful, the genmove() algorithm returns
        the move pre_search() returns. If unsuccessfull, search() is
        called. Default implementation does nothing.
        
        @param brd
        @param game_state
        @param color
        @param consider Moves to consider in this state. Can be
               modified. Passed into search().
        @param max_time
        @param score
        @return INVALID_POINT on failure, otherwise a valid move on
        success.
    */
    virtual HexPoint pre_search(HexBoard& brd, const Game& game_state,
				HexColor color, bitset_t& consider,
                                double max_time, double& score);

    /** Generates a move in the given gamestate. Derived players
        must implement this method. Score can be stored in score.
        @param brd
        @param game_state
        @param color
        @param consider Moves to consider in this state. 
        @param max_time
        @param score
        @return The move to play.
    */
    virtual HexPoint search(HexBoard& brd, const Game& game_state,
			    HexColor color, const bitset_t& consider,
                            double max_time, double& score) = 0;
    
private:
    HexPoint init_search(HexBoard& brd, HexColor color, 
                         bitset_t& consider, double& score);
};

//----------------------------------------------------------------------------

/** Abstract base class for classes adding functionality to
    BenzenePlayer. */
class BenzenePlayerFunctionality : public BenzenePlayer
{
public:
    /** Constructor.
        @param player The player to extend. 
    */
    explicit BenzenePlayerFunctionality(BenzenePlayer* player);

    /** Destructor. */
    virtual ~BenzenePlayerFunctionality();

    /** Returns pointer the player it is extending. */
    BenzenePlayer* PlayerExtending();

    /** Returns name of player it is extending. */
    std::string name() const;

    /** Extends BenzenePlayer::pre_search(). If this implementation
        fails, pre_search() should call pre_search() of player it is
        extending.  In this way, multiple functionalities can be
        chained together.  If you want to simply constrain the moves
        to consider, alter it, and return player->pre_search().
    */
    virtual HexPoint pre_search(HexBoard& brd, const Game& game_state,
				HexColor color, bitset_t& consider,
                                double max_time, double& score) = 0;

protected:
    /** Calls search() method of player it is extending. */
    HexPoint search(HexBoard& brd, const Game& game_state,
		    HexColor color, const bitset_t& consider,
                    double max_time, double& score);

    BenzenePlayer* m_player;
};

inline 
BenzenePlayerFunctionality::BenzenePlayerFunctionality(BenzenePlayer* player)
    : m_player(player)
{
}

inline BenzenePlayerFunctionality::~BenzenePlayerFunctionality()
{
    delete m_player;
}

inline BenzenePlayer* BenzenePlayerFunctionality::PlayerExtending()
{
    return m_player;
}

inline std::string BenzenePlayerFunctionality::name() const
{
    return m_player->name();
}

inline HexPoint 
BenzenePlayerFunctionality::search(HexBoard& brd, const Game& game_state,
                                   HexColor color, const bitset_t& consider,
                                   double max_time, double& score)
{
    return m_player->search(brd, game_state, color, consider,
			    max_time, score);
}

//----------------------------------------------------------------------------

/** Utilities on BenzenePlayers. */
namespace BenzenePlayerUtil
{
    /** Searches through the player decorators to find an instance
        of type T. Returns 0 on failure. */
    template<typename T> T* GetInstanceOf(BenzenePlayer* player);
}

//----------------------------------------------------------------------------

template<typename T> T* BenzenePlayerUtil::GetInstanceOf(BenzenePlayer* player)
{
    T* obj = dynamic_cast<T*>(player);
    if (obj)
        return obj;
    BenzenePlayerFunctionality* func 
        = dynamic_cast<BenzenePlayerFunctionality*>(player);
    if (func)
        return GetInstanceOf<T>(func->PlayerExtending());
    return 0;
}

//----------------------------------------------------------------------------

_END_BENZENE_NAMESPACE_

#endif // BENZENEPLAYER_HPP
