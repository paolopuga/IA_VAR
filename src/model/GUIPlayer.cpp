# include "../../include/model/Parchis.h"
# include "../../include/model/GUIPlayer.h"


GUIPlayer::GUIPlayer(const string& name) : AIPlayer(name){
   this->gui = NULL;
   this->auto_think = false;
   this->next_move_color = none;
   this->next_move_id = -1;
   this->next_move_dice = -1;
   this->next_move_confirmed = false;
   this->think_next_move = false;
}

GUIPlayer::GUIPlayer(const string& name, ParchisGUI& gui) : AIPlayer(name){
   this->gui = &gui;
   this->auto_think = false;
   this->next_move_color = none;
   this->next_move_id = -1;
   this->next_move_dice = -1;
   this->next_move_confirmed = false;
   this->think_next_move = false;
}

GUIPlayer::GUIPlayer(const string& name, int id) : AIPlayer(name, id){
   this->gui = NULL;
   this->auto_think = false;
   this->next_move_color = none;
   this->next_move_id = -1;
   this->next_move_dice = -1;
   this->next_move_confirmed = false;
   this->think_next_move = false;
}

GUIPlayer::GUIPlayer(const string& name, int id, ParchisGUI& gui) : AIPlayer(name, id){
   this->gui = &gui;
   this->auto_think = false;
   this->next_move_color = none;
   this->next_move_id = -1;
   this->next_move_dice = -1;
   this->next_move_confirmed = false;
   this->think_next_move = false;
}


void GUIPlayer::perceive(Parchis& p){
   AIPlayer::perceive(p);

   // cout << "guiturn" << endl;
   // cout << gui->gui_turn << endl;
   // cout << "actual" << endl;
   // cout << actual->getTurn() << endl;
   if (gui->gui_turn >= actual->getTurn()){
      // cout << "Old move received. Already up to date." << endl;
   }
   else if (gui->gui_turn + 1 < actual->getTurn()){
      // cout << gui->gui_turn << " " << actual->getTurn() << endl;
      cout << COUT_RED_BOLD <<
         "Ups. Esto no debería haber pasado. Me he perdido algún turno. Escribe a los profesores para informar del problema :("
         << COUT_NOCOLOR << endl;
   }
   else{
      // cout << "New move received. Updating GUI" << endl;
      vector<tuple<color, int, Box, Box>> last_moves = gui->model->getLastMoves();
      tuple<color, int, int> last_action = gui->model->getLastAction();
      // cout << "Last action: " << str(get<0>(last_action)) << " " << get<1>(last_action) << " " << get<2>(last_action) <<
      //   endl;
      gui->gui_turn++;
      //gui->last_dice = actual->getLastDice();
      gui->queueTurnsArrow(actual->getCurrentColor());
      gui->updateSprites();
      //gui->selectAction(get<0>(last_action), get<2>(last_action), true);
      if (actual->isShockMove()){
         gui->playShockSound();
      }
      else if (actual->isBooMove()){
         gui->playBooSound();
      }

      for (int i = 0; i < last_moves.size(); i++){
         color col = get<0>(last_moves[i]);
         int id = get<1>(last_moves[i]);
         Box origin = get<2>(last_moves[i]);
         Box dest = get<3>(last_moves[i]);

         void (ParchisGUI::*callfront)(void) = NULL;
         void (ParchisGUI::*callback)(void) = NULL;
         if (i == 0 and actual->isBulletMove()) callfront = &ParchisGUI::playBulletSound;
         else if (i == 0 and actual->isHornMove()) callfront = &ParchisGUI::playHornSound;
         else if (i == 0) callfront = &ParchisGUI::playMoveSound;
         if (i == 0 && actual->isEatingMove()){
            callfront = &ParchisGUI::playMoveSound;
            callback = &ParchisGUI::playEatenSound;
         }
         if (i == 0 && actual->goalBounce()){
            callfront = &ParchisGUI::playMoveSound;
            callback = &ParchisGUI::playBoingSound;
         }
         if (i == 0 && actual->isGoalMove()){
            callfront = &ParchisGUI::playApplauseSound;
         }
         if (i == 1 && actual->isEatingMove() && actual->goalBounce()){
            callfront = NULL;
            callback = &ParchisGUI::playEatenSound;
         }
         if (actual->isRedShellMove() or actual->isBlueShellMove()){
            callfront = &ParchisGUI::playExplosionSound;
         }
         if (actual->isStarMove() and !dest.type == home and i < last_moves.size() - 1){
            callback = &ParchisGUI::playStarhitSound;
         }

         if (callfront != NULL) (gui->*callfront)();
         gui->queueMove(col, id, origin, dest, callback);
      }
   }
}

bool GUIPlayer::move(){
   cout << COUT_ORANGE_BOLD << "Esperando a que el jugador elija movimiento..." << COUT_NOCOLOR << endl;

   while (!this->next_move_confirmed && !this->think_next_move && !this->auto_think && !actual->gameOver()){
      sleep(milliseconds(10)); // espera 10 milisegundos (para no saturar la CPU)
   }

   if (actual->gameOver()) return false;

   gui->notPlayableLock(true);

   // Movimiento a través de la GUI
   if (this->next_move_confirmed){
      cout << COUT_ORANGE_BOLD << "El jugador eligió el siguiente movimiento: " << str(this->next_move_color) << " " << this->next_move_id
         << " " << this->next_move_dice << COUT_NOCOLOR << endl;
      actual->movePiece(next_move_color, next_move_id, next_move_dice);
      this->next_move_confirmed = false;
   }

   // Movimiento automático con el botón de la heurística (una sola vez).
   if (this->think_next_move){
      this->think_next_move = false;
      AIPlayer::move();
   }

   // Movimiento automático permanente.
   if (this->auto_think){
      AIPlayer::move();
   }


   return true;
}

void GUIPlayer::setNextMoveColor(color c){
   this->next_move_color = c;
}

void GUIPlayer::setNextMoveId(int id){
   this->next_move_id = id;
}

void GUIPlayer::setNextMoveDice(int dice){
   this->next_move_dice = dice;
}

void GUIPlayer::setNextMove(color c, int id, int dice){
   setNextMoveColor(c);
   setNextMoveId(id);
   setNextMoveDice(dice);
}

void GUIPlayer::confirmNextMove(){
   this->next_move_confirmed = true;
}

void GUIPlayer::thinkNextMove(){
   this->think_next_move = true;
}

bool GUIPlayer::readyForNextTurn(){
   if (actual->gameOver()) return true;
   if (!gui->animationsRunning()){
      gui->animationLock(false);
      gui->last_dice = -1;
      gui->updateSprites();
      return true;
   }
   return false;
}
