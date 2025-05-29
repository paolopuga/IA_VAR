# include "AIPlayer.h"
# include "../../include/model/Parchis.h"

const float masinf = 9999999999.0, menosinf = -9999999999.0;
const float gana = masinf / 10.f, pierde = menosinf / 10.f;
const int num_pieces = 2;
const int PROFUNDIDAD_MINIMAX = 4; // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

void AIPlayer::loadParams() {
    std::ifstream file("bonuses.csv");
    if (!file.is_open()) {
        std::cerr << "Error: no se pudo abrir bonuses.csv" << std::endl;
        return;
    }
    std::string line;
    std::getline(file, line); // cabecera
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key, val;
        if (std::getline(ss, key, ',') && std::getline(ss, val)) {
            try {
                _params[key] = std::stof(val);
            } catch (...) {
                std::cerr << "Formato inválido en CSV: " << line << std::endl;
            }
        }
    }
}

float AIPlayer::getParam(const std::string &name, float def) {
    auto it = _params.find(name);
    return (it != _params.end() ? _params[name] : def);
}


bool AIPlayer::move(){
   cout << COUT_ORANGE_BOLD << "Realizo un movimiento automatico" << COUT_NOCOLOR << endl;

   color c_piece;
   int id_piece;
   int dice;
   think(c_piece, id_piece, dice);

   cout << COUT_ORANGE_BOLD << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << COUT_NOCOLOR << endl;

   actual->movePiece(c_piece, id_piece, dice);
   return true;
}

void AIPlayer::think(color& c_piece, int& id_piece, int& dice) const{

   float valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
   float alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
   // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
   ValoracionTest valoracionTest;
   Mejora1 mejora1;

   // Si quiero poder manejar varios comportamientos, puedo usar la variable id del agente para usar una u otra.
   switch (id)
   {
   case 0:
      // Mi implementación base de la poda con ValoracionTest
      valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &mejora1);
      break;
   case 1:
      // Mi implementación definitiva con la que gano a todos los ninjas.
      //valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest);
      break;
   case 2:
      // Las distintas pruebas que he realizado (primera prueba)
      //valor = Poda_AlfaBeta_ProfDin(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest);
      break;
   case 3:
      // Las distintas pruebas que he realizado (segunda prueba)
      //valor = Poda_AlfaBeta_Orden(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest);
      break;
   case 4:
      // Las distintas pruebas que he realizado (tercera prueba)
      //valor = Poda_AlfaBeta_Probab(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, &valoracionTest, 0.1);
      break;
   }

}

/****************** PODAS ******************/

float Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max,
   color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic){
// Caso base: si llegamos al límite de profundidad o el juego ha terminado
   if (profundidad == profundidad_max || actual.gameOver()) {
      return heuristic->evaluate(actual, jugador);
   }

   bool soyMax = actual.getCurrentPlayerId() == jugador; //para saber si es un nodo max
   double valor; //valor que devuelve el hijo
   ParchisBros hijos = actual.getChildren();//hijos
   for(auto it = hijos.begin(); it != hijos.end(); ++it){
        //vemos que valor nos devuelve el hijo
        valor = Poda_AlfaBeta(*it,jugador,profundidad+1,profundidad_max,c_piece,id_piece,dice,alpha,beta,heuristic);

        if(soyMax){//si somos max

         if (NodeCounter::isLimitReached()) {
            std::cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << std::endl;
            if (profundidad == 0) {
               c_piece = it.getMovedColor();
               id_piece = it.getMovedPieceId();
               dice = it.getMovedDiceValue();
            }
            return alpha;
         }


         if(alpha < valor){//si el nuevo valor es mejor que alpha lo actualizamos
               alpha = valor;
               if(profundidad == 0){//para que se actualice la jugada
                  c_piece = it.getMovedColor();
                  id_piece = it.getMovedPieceId();
                  dice = it.getMovedDiceValue();
               }
               }
               if(alpha >= beta){//si se poda
                  break;
               }
        }
        else{//si es min

         if (NodeCounter::isLimitReached()) {
            std::cout << "Límite de nodos alcanzado, devolviendo el mejor nodo parcial" << std::endl;
            return heuristic->evaluate(actual, jugador);
         }
         if(beta > valor)//si el valor es menor que beta beta se actualiza
               beta = valor;

         if(beta <= alpha)//si se poda
               break;
        }
    }

    if(soyMax)//si somos max devolvemos alpha, si no beta
        return alpha;
    else
        return beta; 
}

float Poda_AlfaBeta_ProfDin(const Parchis &actual, int jugador, int profundidad, int profundidad_max,
   color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic){

}

float Poda_AlfaBeta_Orden(const Parchis &actual, int jugador, int profundidad, int profundidad_max,
   color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic){

}

float Poda_AlfaBeta_Probab(const Parchis &actual, int jugador, int profundidad, int profundidad_max,
   color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic, float epsilon_prob){

}


/****************** HEURISTICAS ******************/

float ValoracionTest::getHeuristic(const Parchis& estado, int jugador) const{
   // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


   int ganador = estado.getWinner();
   int oponente = (jugador + 1) % 2;

   // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
   if (ganador == jugador){
      return gana;
   }
   else if (ganador == oponente){
      return pierde;
   }
   else{
      // Colores que juega mi jugador y colores del oponente
      vector<color> my_colors = estado.getPlayerColors(jugador);
      vector<color> op_colors = estado.getPlayerColors(oponente);

      // Recorro todas las fichas de mi jugador
      int puntuacion_jugador = 0;
      // Recorro colores de mi jugador.
      for (int i = 0; i < my_colors.size(); i++){
         color c = my_colors[i];
         // Recorro las fichas de ese color.
         for (int j = 0; j < num_pieces; j++){
            // Valoro positivamente que la ficha esté en casilla segura o meta.
            if (estado.isSafePiece(c, j)){
               puntuacion_jugador++;
            }
            else if (estado.getBoard().getPiece(c, j).get_box().type == goal){
               puntuacion_jugador += 5;
            }
         }
      }

      // Recorro todas las fichas del oponente
      int puntuacion_oponente = 0;
      // Recorro colores del oponente.
      for (int i = 0; i < op_colors.size(); i++){
         color c = op_colors[i];
         // Recorro las fichas de ese color.
         for (int j = 0; j < num_pieces; j++){
            if (estado.isSafePiece(c, j)){
               // Valoro negativamente que la ficha esté en casilla segura o meta.
               puntuacion_oponente++;
            }
            else if (estado.getBoard().getPiece(c, j).get_box().type == goal){
               puntuacion_oponente += 5;
            }
         }
      }

      // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
      return puntuacion_jugador - puntuacion_oponente;
   }
}

float Mejora1::getHeuristic(const Parchis& estado, int jugador) const{
   int ganador = estado.getWinner();
   int oponente = (jugador + 1) % 2;

   // Parámetros de victoria/derrota
   if (ganador == jugador)   return gana;
   if (ganador == oponente)  return pierde;

   // Pesos dinámicos desde CSV
   float wSafe    = AIPlayer::getParam("safe", 1.0f);
   float wGoal    = AIPlayer::getParam("goal", 5.0f);
   float wOppSafe = AIPlayer::getParam("opp_safe", 1.0f);
   float wOppGoal = AIPlayer::getParam("opp_goal", 5.0f);

   int score = 0;
   // Mis fichas
   for (auto c : estado.getPlayerColors(jugador)) {
      for (int i = 0; i < num_pieces; ++i) {
         if (estado.isSafePiece(c, i)) score += int(wSafe);
         if (estado.getBoard().getPiece(c,i).get_box().type == goal)
               score += int(wGoal);
      }
   }
   // Fichas oponente
   for (auto c : estado.getPlayerColors(oponente)) {
      for (int i = 0; i < num_pieces; ++i) {
         if (estado.isSafePiece(c, i)) score -= int(wOppSafe);
         if (estado.getBoard().getPiece(c,i).get_box().type == goal)
               score -= int(wOppGoal);
      }
   }
   return float(score);
}
   