#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Vertice {
    char id;
    int heuristica;
    vector<pair<Vertice*, int>> adyacentes;
};

class Grafo {
public:
    void agregar_vertice(char id, int heuristica) {
        vertices[id] = Vertice{id, heuristica, {}};
    }

    void agregar_arista(char id1, char id2, int costo) {
        vertices[id1].adyacentes.push_back({&vertices[id2], costo});
    }

    struct EstructuraCamino {
        vector<char> camino;
        int costo;
        int nodos_expandidos;
        unordered_map<char, int> contador_expansion_nodos;
    };

    EstructuraCamino busqueda_costo_uniforme(char inicio_id, char objetivo_id) {
        unordered_map<char, int> distancias;                                                                //Almancena las distancias acumuladas
        unordered_map<char, char> anterior;                                                                 //Almacena el nodo anterior el camino para cada nodo
        priority_queue<pair<int, char>, vector<pair<int, char>>, greater<pair<int, char>>> cola_prioridad;  //Almaecna los nodos explorados, prioridad por distancia acumulada
        int nodos_expandidos = 0;                                                                           //Numero total de nodos expandidos
        unordered_map<char, int> contador_expansion_nodos;                                                  //Cantidad de veces que un nodo fue expandido

        for (const auto& par_vertice : vertices) {
            distancias[par_vertice.first] = numeric_limits<int>::max();                                     //En un principio todas las distancias tiene un valor infinito
        }

        distancias[inicio_id] = 0;
        cola_prioridad.push({0, inicio_id});

        while (!cola_prioridad.empty()) {                                //Si hay nodos por explorar en la queue, se obtiene el nodo con menor distancia acumulada, es eliminado de la queue    
            char id_actual = cola_prioridad.top().second;                  // y es actualizado el valor de nodos expandidos y el contador de expansion del nodo actual
            cola_prioridad.pop();
            nodos_expandidos++;
            contador_expansion_nodos[id_actual]++;

            if (id_actual == objetivo_id) {                               //Si el nodo actual es el obj se termina la busqueda.
                break;
            }

            for (const auto& par_adyacente : vertices[id_actual].adyacentes) { //Para cada nodo adyacente al nodo actual, calcula el nuevo costo del camino desde el nodo de inicio hasta el nodo adyacente pasando por el nodo actual.
                Vertice* adyacente = par_adyacente.first;
                int costo = par_adyacente.second;
                int nuevo_costo = distancias[id_actual] + costo;

                if (nuevo_costo < distancias[adyacente->id]) {              //Si el nuevo costo del camino es < que el actual para el adyecente
                    distancias[adyacente->id] = nuevo_costo;                
                    anterior[adyacente->id] = id_actual;                    // guarda el nodo actual como el nodo anterior en el camino para el nodo adyacente 
                    cola_prioridad.push({nuevo_costo, adyacente->id});      // y agrega el nodo adyacente a la queue con el nuevo costo.
                }
            }
        }

        vector<char> camino;
        int costo_camino = -1;                                              // si el costo al final sigue siendo  -1, no se encontro un camino
        if (anterior.find(objetivo_id) != anterior.end()) {                 // si se encontró un camino al nodo objetivo
            char id_actual = objetivo_id;                                   // se construye el camino desde el nodo obj hasta el nodo inicial
            while (id_actual != inicio_id) {
                camino.push_back(id_actual);                            
                id_actual = anterior[id_actual];
            }
            camino.push_back(inicio_id);
            reverse(camino.begin(), camino.end());                          // Se invierte para que vaya desde el nodo de inicio hasta el nodo objetivo y asigna el costo del camino
            costo_camino = distancias[objetivo_id];
        }

        return {camino, costo_camino, nodos_expandidos, contador_expansion_nodos};
    }

private:
    unordered_map<char, Vertice> vertices;
};
int main() {                    //Estructure el parseo de forma diferente en algunos codigos netamente por experimentacion                    
    Grafo grafo;
    ifstream archivo("grafo.txt");
    string linea;
    char nodo_inicio, nodo_objetivo;

    if (archivo.is_open()) {        // se leen el nodo inicial y el nodo objetivo
        if (getline(archivo, linea)) {
            stringstream ss(linea);
            string tmp;
            ss >> tmp >> nodo_inicio;
        }
        if (getline(archivo, linea)) {
            stringstream ss(linea);
            string tmp;
            ss >> tmp >> nodo_objetivo;
        }

        
        while (getline(archivo, linea) && linea.find(',') == string::npos) { // Se leen los vértices con sus heurísticas
            stringstream ss(linea);
            char nodo;
            int heuristica;
            ss >> nodo >> heuristica;
            grafo.agregar_vertice(nodo, heuristica);
        }

        
        do {        // Se leen las aristas y sus costos
            stringstream ss(linea);
            char nodo1, nodo2;
            int costo;
            ss >> nodo1;
            ss.ignore(1, ',');
            ss >> nodo2;
            ss.ignore(1, ',');
            ss >> costo;
            grafo.agregar_arista(nodo1, nodo2, costo);
        } while (getline(archivo, linea));

        archivo.close();
    } else {
        cerr << "Error al abrir el archivo." << endl;
        return 1;
    }

    Grafo::EstructuraCamino resultado = grafo.busqueda_costo_uniforme(nodo_inicio, nodo_objetivo);

    cout << "Camino encontrado: " << nodo_inicio;
    for (char id : resultado.camino) {
        if (id != nodo_inicio) {
            cout << " > " << id;
        }
    }
    cout << endl;

    cout << "Costo: " << resultado.costo << endl;

    for (const auto& par_expansion_nodos : resultado.contador_expansion_nodos) {
        cout << par_expansion_nodos.first << ": " << par_expansion_nodos.second << endl;
    }

    return 0;
}