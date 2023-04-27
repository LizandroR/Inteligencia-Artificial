#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <stack>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Vertice {
char id;
int heuristica;
};

struct Arista {
Vertice origen;
Vertice destino;
int coste;
};

class Grafo {
public:
Grafo() {
srand(time(NULL));
}
void agregarVertice(char id, int heuristica) {
    vertices[id] = Vertice{id, heuristica};
}

void agregarArista(char origen, char destino, int coste) {
    if (vertices.find(origen) != vertices.end() && vertices.find(destino) != vertices.end()) {
        Arista arista = {vertices[origen], vertices[destino], coste};
        aristas.push_back(arista);
    }
}

tuple<vector<char>, int, unordered_map<char, int>> busquedaEnProfundidad(char nodoInicial, char nodo_objetivo) {
    stack<char> pila;
    vector<char> visitados;
    unordered_map<char, char> predecesores;
    unordered_map<char, int> costes;
    unordered_map<char, int> nodosExpandidos;

    pila.push(nodoInicial);                 // Se inicializa la pila con el nodo inicial, marca el nodo inicial como visitado.
    visitados.push_back(nodoInicial);
    costes[nodoInicial] = 0;

    while (!pila.empty()) {
        char nodoActual = pila.top();
        pila.pop();
        nodosExpandidos[nodoActual]++;

        if (nodoActual == nodo_objetivo) {   //Si el nodo actual es el nodo objetivo, construye y retorna el camino, el costo y los nodos expandidos.
            return make_tuple(construirCamino(predecesores, nodoInicial, nodo_objetivo),
                                costes[nodo_objetivo], nodosExpandidos);
        }

        vector<char> sucesores = obtenerSucesoresAlAzar(nodoActual);            // Se obtienen los sucesores del nodo actual en orden aleatorio.
        for (char sucesor : sucesores) {                                        // Se recorren los sucesores del nodo actual.
            if (find(visitados.begin(), visitados.end(), sucesor) == visitados.end()) {         // Si el sucesor no ha sido visitado, lo marca como visitado, actualiza su predecesor
                visitados.push_back(sucesor);                                                       // Se actualiza el costo y lo agrega a la pila 
                predecesores[sucesor] = nodoActual;
                costes[sucesor] = costes[nodoActual] + obtenerCosteArista(nodoActual, sucesor);
                pila.push(sucesor);
            }
        }
    }

    return make_tuple(vector<char>(), 0, nodosExpandidos);
}
private:
unordered_map<char, Vertice> vertices;
vector<Arista> aristas;
vector<char> obtenerSucesoresAlAzar(char id) {
    vector<char> sucesores;
    for (Arista& arista : aristas) {        
        if (arista.origen.id == id) {           // Si el origen de la arista coincide con el identificador del nodo, agrega el nodo destino a la lista de sucesores.
            sucesores.push_back(arista.destino.id);
        }
    }

    random_shuffle(sucesores.begin(), sucesores.end());
    return sucesores;
}

int obtenerCosteArista(char origen, char destino) {     // Si el origen y el destino de la arista coinciden con los nodos proporcionados, retorna el coste de la arista.
    for (Arista& arista : aristas) {
        if (arista.origen.id == origen && arista.destino.id == destino) {
            return arista.coste;
        }
    }
    return 0;
}

vector<char> construirCamino(unordered_map<char, char>& predecesores, char nodoInicial, char nodo_objetivo) {   // Se construye el camino desde el nodo inicial hasta el nodo objetivo
    vector<char> camino;
char nodoActual = nodo_objetivo;
    while (nodoActual != nodoInicial) {
        camino.push_back(nodoActual);
        nodoActual = predecesores[nodoActual];
    }
    camino.push_back(nodoInicial);
    reverse(camino.begin(), camino.end());
    return camino;
}
};

int main() {        
Grafo grafo;
ifstream archivo("grafo.txt");

if (!archivo.is_open()) {
    cerr << "No se pudo abrir el archivo." << endl;
    return 1;
}

char nodoInicial, nodo_objetivo;
string linea;

getline(archivo, linea);
sscanf(linea.c_str(), "Init: %c", &nodoInicial);

getline(archivo, linea);
sscanf(linea.c_str(), "Goal: %c", &nodo_objetivo);

while (getline(archivo, linea)) {
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

archivo.close();

tuple<vector<char>, int, unordered_map<char, int>> resultado = grafo.busquedaEnProfundidad(nodoInicial, nodo_objetivo);
vector<char> camino = get<0>(resultado);
int costo = get<1>(resultado);
unordered_map<char, int> nodosExpandidos = get<2>(resultado);

if (!camino.empty()) {
    cout << "Camino encontrado desde " << nodoInicial << " hasta " << nodo_objetivo << ":\n";
    for (size_t i = 0; i < camino.size(); ++i) {
        cout << camino[i];
        if (i < camino.size() - 1) {
            cout << ">";
        }
    }
    cout << "\nCosto: " << costo << endl;
    for (const auto& nodo : nodosExpandidos) {
        cout << nodo.first << ": " << nodo.second << endl;
    }
} else {
    cout << "No se encontró un camino desde " << nodoInicial << " hasta " << nodo_objetivo << endl;
}

return 0;
}