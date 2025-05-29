#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

# include "../../include/model/Attributes.h"
# include "../../include/model/Player.h"
# include "../../include/model/Heuristic.h"
# include <unordered_map>
# include <fstream>
# include <sstream>
# include <iostream>


class ValoracionTest: public Heuristic{
   protected:
      virtual float getHeuristic(const Parchis &estado, int jugador) const;
};

class Mejora1: public Heuristic{
   protected:
      virtual float getHeuristic(const Parchis &estado, int jugador) const;
};

class Mejora2: public Heuristic{
   protected:
      virtual float getHeuristic(const Parchis &estado, int jugador) const;
};

class Mejora3: public Heuristic{
   protected:
      virtual float getHeuristic(const Parchis &estado, int jugador) const;
};

class AIPlayer : public Player{
protected:
   //Id identificativo del jugador
   const int id;

private:
   inline static std::unordered_map<std::string, float> _params;
   static void loadParams();  // carga "bonuses.csv"

public:

   // Llamar una vez al iniciar el programa
   static void initParams(const std::string &csvPath);

   // Obtener cualquier parámetro por nombre
   static float getParam(const std::string &name, float def = 0.f);


   /**
    * @brief Constructor de un objeto AIPlayer
    *
    * @param name Nombre del jugador
    */
   inline AIPlayer(const string& name): Player(name), id(1){
      if (_params.empty()) {
         loadParams();  // Carga los parámetros desde el CSV si aún no se ha hecho
      }
   };

   /**
    * @brief Constructor de un objeto AIPlayer
    *
    * @param name Nombre  del jugador
    * @param id Id del jugador
    */
   inline AIPlayer(const string& name, const int id): Player(name), id(id){
      if (_params.empty()) {
         loadParams();  // Carga los parámetros desde el CSV si aún no se ha hecho
      }
   };

   /**
    * @brief Función que percibe el el parchís y al jugador actual.
    * Asigna el tablero en actual y el id del jugador.
    *
    * @param p Instancia Parchis que se quiere percibir
    */
   inline virtual void perceive(Parchis& p){ Player::perceive(p); }

   /**
    * @brief Función abstracta que define el movimiento devuelto por el jugador.
    * Llama a la función movePiece con el valor asignado a los parámetros pasados
    * por referencia.
    *
    * @return true
    * @return false
    */
   virtual bool move();

   /**
    * @brief Función que se encarga de decidir el mejor movimiento posible a
    * partir del estado actual del tablero. Asigna a las variables pasadas por
    * referencia el valor de color de ficha, id de ficha y dado del mejor movimiento.
    *
    * @param c_piece Color de la ficha
    * @param id_piece Id de la ficha
    * @param dice Número de dado
    */
   virtual void think(color& c_piece, int& id_piece, int& dice) const;

   /**
    * @brief Método que determina si el player es inteligente (decide el mejor movimiento)
    * o no. True para AIPlayer.
    *
    * @return true
    * @return false
    */
   inline virtual bool canThink() const{ return true; }
   
};

/**
 * @brief Propuesta de declaración de la función poda alfa-beta.
 * La propuesta es solo sugerencia, los parámetros de la declaración podrían variar.
 */
// float Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic);
float Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, int profundidad_max,
   color &c_piece, int &id_piece, int &dice, float alpha, float beta, Heuristic *heuristic);





#endif
