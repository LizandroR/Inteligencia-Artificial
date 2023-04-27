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

    EstructuraCamino busqueda_a_estrella(char inicio_id, char objetivo_id) { // la documentacion de todo esta en la implementacion de costo uniforme, ya que es casi lo mismo que A*
        unordered_map<char, int> distancias;
        unordered_map<char, char> anterior;
        priority_queue<pair<int, char>, vector<pair<int, char>>, greater<pair<int, char>>> cola_prioridad;
        int nodos_expandidos = 0;
        unordered_map<char, int> contador_expansion_nodos;

        for (const auto& par_vertice : vertices) {
            distancias[par_vertice.first] = numeric_limits<int>::max();
        }

        distancias[inicio_id] = 0;
        cola_prioridad.push({vertices[inicio_id].heuristica, inicio_id});

        while (!cola_prioridad.empty()) {
            char id_actual = cola_prioridad.top().second;
            cola_prioridad.pop();
            nodos_expandidos++;
            contador_expansion_nodos[id_actual]++;

            if (id_actual == objetivo_id) {
                break;
            }

            for (const auto& par_adyacente : vertices[id_actual].adyacentes) {
                Vertice* adyacente = par_adyacente.first;
                int costo = par_adyacente.second;
                int nuevo_costo = distancias[id_actual] + costo;

                if (nuevo_costo < distancias[adyacente->id]) {
                    distancias[adyacente->id] = nuevo_costo;
                    anterior[adyacente->id] = id_actual;
                    
                    int fn = nuevo_costo + adyacente->heuristica; // Se suma el costo actual y la heurística del nodo adyacente para obtener el valor de f(n) = g(n) + h(n)
                    cola_prioridad.push({fn, adyacente->id});
                }
            }
        }

        vector<char> camino;
        int costo_camino = -1;
        if (anterior.find(objetivo_id) != anterior.end()) {
            char id_actual = objetivo_id;
            while (id_actual != inicio_id) {
                camino.push_back(id_actual);
                id_actual = anterior[id_actual];
            }
            camino.push_back(inicio_id);
            reverse(camino.begin(), camino.end());
            costo_camino = distancias[objetivo_id];
        }

        return {camino, costo_camino, nodos_expandidos, contador_expansion_nodos};
    }

private:
    unordered_map<char, Vertice> vertices;
};

int main() {
    Grafo grafo;
    ifstream archivo("grafo.txt");
    string linea;
    char nodo_inicio, nodo_objetivo;

    if (archivo.is_open()) {
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

        while (getline(archivo, linea) && linea.find(',') == string::npos) {
            stringstream ss(linea);
            char nodo;
            int heuristica;
            ss >> nodo >> heuristica;
            grafo.agregar_vertice(nodo, heuristica);
        }

        do {
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

    Grafo::EstructuraCamino resultado = grafo.busqueda_a_estrella(nodo_inicio, nodo_objetivo);

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