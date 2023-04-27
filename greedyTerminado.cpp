#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <limits>
#include <fstream>
#include <sstream>
#include <tuple>
#include <algorithm>

using namespace std;

struct Vertice {
    char id;
    int heuristica;
};

struct Arista {
    Vertice destino;
    int coste;
};


class Grafo {
    map<char, Vertice> vertices;                  // Mapa que almacena los vertices del grafo
    map<char, vector<Arista>> aristas;            // Mapa que almacena las aristas del grafo

public:
    // Funcion para agregar un vértice al grafo
    void agregarVertice(char id, int heuristica) {
        Vertice v{id, heuristica};
        vertices[id] = v;
    }

    // Funcion para agregar una arista al grafo
    void agregarArista(char origen, char destino, int coste) {
        Vertice vertice_destino = vertices[destino];
        Arista arista{vertice_destino, coste};
        aristas[origen].push_back(arista);
    }

    // Funcion que implementa el algoritmo Greedy para encontrar un camino en el grafo
    tuple<vector<char>, int, map<char, int>> algoritmoGreedy(char nodoInicial, char nodo_objetivo) { 
        auto comparador = [](const Vertice& a, const Vertice& b) { return a.heuristica > b.heuristica; };// Se comparan dos vertices para darle prioridad al de menor heuristica
        priority_queue<Vertice, vector<Vertice>, decltype(comparador)> colaPrioridad(comparador);            // Almaecena nodos pendientes 
        map<char, char> predecesor;     //  Almacena el nodo predecesor de cada nodo en el camino encontrado
        map<char, int> costos;          //  Almacena el costo acumulado hasta cada nodo visitado
        map<char, int> nodosExpandidos; //  Almacena la cantidad de veces que cada nodo ha sido expandido.

        colaPrioridad.push(vertices[nodoInicial]);
        costos[nodoInicial] = 0;

        while (!colaPrioridad.empty()) {
            Vertice actual = colaPrioridad.top(); // Se extrae el nodo con la h. mas baja
            colaPrioridad.pop();
            nodosExpandidos[actual.id]++;

            if (actual.id == nodo_objetivo) {    //Si el nodo actual es el nodo objetivo, sale del bucle
                break;
            }

            for (const Arista& arista : aristas[actual.id]) {
                if (predecesor.find(arista.destino.id) == predecesor.end()) { // Si el nodo destino de la arista no ha sido visitado, se actualiza el predecesor del nodo destino con el nodo actual
                    predecesor[arista.destino.id] = actual.id;                  //y se calcula el costo acumulado hasta el nodo destino.
                    costos[arista.destino.id] = costos[actual.id] + arista.coste;
                    colaPrioridad.push(arista.destino);
                }
            }
        }

        if (predecesor.find(nodo_objetivo) == predecesor.end()) {    //Si el nodo objetivo no tiene un predecesor, significa que no se encontro un camino.
            return make_tuple(vector<char>(), -1, nodosExpandidos);
        }

        vector<char> camino;                //Se construye el camino encontrado, recorriendo los predecesores desde el nodo objetivo hasta el nodo inicial y almacenándolos en un vector llamado camino.
        char actual = nodo_objetivo;
        while (actual != nodoInicial) {
            camino.push_back(actual);
            actual = predecesor[actual];
        }
        camino.push_back(nodoInicial);
        reverse(camino.begin(), camino.end());//Se invierte el vector camino para que muestre el recorrido desde el nodo inicial al nodo objetivo

        return make_tuple(camino, costos[nodo_objetivo], nodosExpandidos); //Se retorna una tupla con el vector camino, el costo del camino y el mapa de nodosExpandidos.
    }
};

int main() {        //Estructure el parseo de forma diferente en los codigos netamente por experimentacion
    Grafo grafo;
    ifstream archivo("grafo.txt");

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        return 1;
    }

    char nodoInicial, nodo_objetivo;
    string linea;

    
    getline(archivo, linea);            // Se lee el nodo inicial y el nodo objetivo desde el archivo
    sscanf(linea.c_str(), "Init: %c", &nodoInicial);

    getline(archivo, linea);
    sscanf(linea.c_str(), "Goal: %c", &nodo_objetivo);

    
    while (getline(archivo, linea)) {      // Se leen los nodos y las aristas desde el archivo y agregarlos al grafo
        if (linea.find(",") != string::npos) {
            char origen, destino;
            int coste;
            sscanf(linea.c_str(), "%c,%c,%d", &origen, &destino, &coste);
            grafo.agregarArista(origen, destino, coste);
        } else {
            char nodo;
            int heuristica;
            sscanf(linea.c_str(), "%c %d", &nodo, &heuristica);
            grafo.agregarVertice(nodo, heuristica);
        }
    }

    // Cerrar el archivo
    archivo.close();

    tuple<vector<char>, int, map<char, int>> resultado = grafo.algoritmoGreedy(nodoInicial, nodo_objetivo);
    vector<char> camino = get<0>(resultado);
    int costo = get<1>(resultado);
    map<char, int> nodosExpandidos = get<2>(resultado);
    cout << "Camino encontrado: ";
    for (size_t i = 0; i < camino.size(); ++i) {
        cout << camino[i];
        if (i < camino.size() - 1) {
            cout << " > ";
        }
    }
    cout << endl;
    cout << "Costo: " << costo << endl;
    cout << "Nodos expandidos:" << endl;
    for (const auto& nodo : nodosExpandidos) {
        cout << nodo.first << ": " << nodo.second << endl;
    }

    return 0;
}
