/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: Josue Alvarez M
 *
 * Created on 25 de marzo de 2022, 17:46
 */

#include <iostream>
#include "rapidxml/rapidxml_ext.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <sstream>
#include <fstream>
#include <bits/stdc++.h>

#include <iostream>
#include <fstream>

#include <codecvt>
#include <fcntl.h>
#include <io.h>

using namespace std;
using namespace rapidxml;

xml_document<> raiz; // Arbol DOM

double* recta(double x1, double y1, double x2, double y2){
    double m = (y2 - y1) / (x2 - x1);
    double b = y1 - (m*x1);
    
    double* recta = new double[2];
    recta[0] = m;
    recta[1] = b;
    
    return recta;
}

double* recta(double* p1, double* p2){
    return recta(p1[0], p1[1], p2[0], p2[1]);
}

char* strToChar(string str){
    int tam = str.size();
    char* arr = new char[tam];

    int i;
    for (i = 0; i < tam; i++) {
        arr[i] = str[i];
    }
    
    return arr;
}

/*
 *  Traza una curve desde p1 hasta p3 acercandose a p2
 */
double** bezierCurve(double* p1, double* p2, double* p3, int cantidad){
    double** puntos = new double*[cantidad];
    
    double* recta1 = recta(p1, p2);
    double* recta2 = recta(p2, p3);
    
    double incXRec1, incYRec1;
    if(abs(p2[0] - p1[0]) > abs(p2[1] - p1[1])){
        incXRec1 = (p2[0] - p1[0]) / cantidad;
        incYRec1 = 0;
    }
    else{
        incXRec1 = 0;
        incYRec1 = (p2[1] - p1[1]) / cantidad;
    }
    
    double incXRec2, incYRec2;
    if(abs(p3[0] - p2[0]) > abs(p3[1] - p2[1])){
        incXRec2 = (p3[0] - p2[0]) / cantidad;
        incYRec2 = 0;
    }
    else{
        incXRec2 = 0;
        incYRec2 = (p3[1] - p2[1]) / cantidad;
    }
    
    // puntos a seleccionar de la recta central
    double x = p1[0];
    double y = p1[1];
    
    double xDif = p3[0] - p1[0];
    double yDif = p3[1] - p1[1];
    double incX, incY;
    if(abs(xDif) > abs(yDif)){
        incX = xDif / cantidad;
        incY = 0;
    }
    else{
        incX = 0;
        incY = yDif / cantidad;
    }
    
    double* rectaCen; // Recta central, con la que se seleccionan los puntos de la curva
    for (int i = 0; i < cantidad; i++) {
        puntos[i] = new double[2];
        
        rectaCen = recta(p1, p2);
        
        if(abs(xDif) > abs(yDif))
            // y = mx + b
            y = (rectaCen[0]*x) + rectaCen[1];
        
        else
            // x = (y - b) / m
            x = (y - rectaCen[1]) / rectaCen[0];
        
        puntos[i][0] = x;
        puntos[i][1] = y;
        
        //cout << p1[0] << "," << p1[1] << ";" << p2[0] << "," << p2[1] << ";" << p3[0] << "," << p3[1] << ":" << i << ":" << x << "," << y << endl;
        //while(true){}
        
        x += incX;
        y += incY;
        
        p1[0] += incXRec1;
        p1[1] += incYRec1;
        p2[0] += incXRec2;
        p2[1] += incYRec2;
        
        if(incXRec1 != 0)
            p1[1] = (recta1[0] * p1[0]) + recta1[1];
        else
            p1[0] = (p1[1] - recta1[1]) / recta1[0];
        
        if(incXRec2 != 0)
            p2[1] = (recta2[0] * p2[0]) + recta2[1];
        else
            p2[0] = (p2[1] - recta2[1]) / recta2[0];
    }
    
    return puntos;
}

double** bezierCurve(double xP1, double yP1, double xP2, double yP2, double xP3, double yP3, int cantidad){
    double* p1 = new double[2];
    p1[0] = xP1;
    p1[1] = yP1;
    
    double* p2 = new double[2];
    p2[0] = xP2;
    p2[1] = yP2;
    
    double* p3 = new double[2];
    p3[0] = xP3;
    p3[1] = yP3;
    
    return bezierCurve(p1, p2, p3, cantidad);
}

class Path{
private:
    xml_node<>* nodoXML;
    
    // puntos de inicio y fin del area
    double areaInicio[2];
    double areaFin[2];
    
    string id = "";
    string opacidad = "";
    
    string color = "#000000";
    int r = 0;
    int g = 0;
    int b = 0;
    
    //coordenada indicada por moveto
    double M_x = -1;
    double M_y = -1;
    double* M = new double(2);
    char M_char;
    
    string d;
    vector<string> vectorD;
    
    // posibles posiciones donde se moverá el path
    vector<double*> posiciones;
    int posTam = 0;
    
public:
    
    void addPosicion(double* pos){
        posiciones.push_back(pos);
        posTam++;
    }
    
    void setNodoXML(xml_node<>* nodoXML) {
        this->nodoXML = nodoXML;
    }

    void setColor(char* color) {
        this->color = color;
        sscanf(color, "#%02x%02x%02x", &r, &g, &b);
    }
    
    void setId(string id) {
        this->id = id;
    }

    void setOpacidad(string opacidad) {
        this->opacidad = opacidad;
    }
    
    void mover(double* M) {
        // Actualiza los puntos del path
        string dato;
        bool absoluta = false; // si la instruccion es absoluta o relativa
        
        int cont = 1;
        
        int tam = this->vectorD.size();
        for (int i = 0; i < tam; i++) {
            dato = this->vectorD[i]; // Selecciona el dato del path (código o num)
            
            if(dato.size() == 1){
                if(int('a') <= int(dato[0]) && int(dato[0]) <= int('z')){
                    absoluta = false;
                    continue;
                }
                else if(int('A') <= int(dato[0]) && int(dato[0]) <= int('Z')){
                    absoluta = true;
                    cont = 1;
                    continue;
                }
            }
            
            if(absoluta){ // El dato es una coordenada x o y
                double num = stod(dato);
                if(cont == 1){ // X
                    this->vectorD[i] = to_string(abs(num) - abs(this->M_x) + M[0]);
                    cont = 2;
                }
                else{ // Y
                    this->vectorD[i] = to_string(abs(num) - abs(this->M_y) + M[1]);
                    cont = 1;
                }
            }
        }
        
        //Actualiza el moveto
        this->M = M;
        this->M_x = M[0];
        this->M_y = M[1];
    }

    
    string getId() const {
        return id;
    }

    string getOpacidad() const {
        return opacidad;
    }
    
    vector<double*> getPosiciones() const {
        return posiciones;
    }
    
    int getPosTam() const {
        return posTam;
    }

    
    double* getPosMoveto() const {
        this->M[0] = M_x;
        this->M[1] = M_y;
        
        return M;
    }
    
    string getColor() const {
        return color;
    }
    int* getColorRGB() const {
        int colorRGB[3] = {r, g, b};
        int* colorRGBPuntero = colorRGB;
        return colorRGBPuntero;
    }
    

    double* getAreaFin(){
        return areaFin;
    }
    double* getAreaInicio(){
        return areaInicio;
    }

    
    bool isArea(double x, double y){
        return x >= this->areaInicio[0] && y >= this->areaInicio[1] && x <= this->areaFin[0] && y <= this->areaFin[1];
    }
    
    /*
     * Pasa el path a un vector de double
     */
    void pasarAVector(){
        string numStr = ""; // donde se gurdan los char del numero a pasar a double
        
        bool punto = false; // si ya se ha agregado al número un '.'
        
        int pos = 0; // posicion a insertar en aux
        double aux[2]; // guarda el punto encontrado y que se debe procesar
        
        int tam = this->d.size();
        for (int i = 0; i < tam; i++) {
            char c = this->d[i];
            
            // Pasa los char del número al string
            if((c == '.' && !punto) || (c == '-' && numStr == "") || (int('0') <= int(c) && int(c) <= int('9'))){
                numStr += c;
                if(c == '.')
                    punto = true;
            }
            else{
                if(numStr != ""){
                    this->vectorD.push_back(numStr);
                    
                    numStr = "";
                    punto = false;
                }
                // si el char es un código
                if((int('A') <= int(c) && int(c) <= int('Z')) || (int('a') <= int(c) && int(c) <= int('z'))){
                    // Guarda el código de instrucción actual
                    string codigo = "";
                    codigo += c;
                    this->vectorD.push_back(codigo);
                }
            }
            
            if(c == '-' && numStr == "")
                numStr += c;
        }
    }
    
    /*
     * Analiza y define el area según la información del path entrante
     */
    void path(string infoPath){
        if(toupper(infoPath[infoPath.size()-1]) != 'Z')
            infoPath += 'z'; // Agrega 'z' para marcar el final del path
        
        this->d = infoPath;
        
        string numStr = ""; // donde se gurdan los char del numero a pasar a double
        char codigoNuevo = ' '; // codigo que se acaba de encontrar
        char codigo = ' '; // codigo el cual se está recorriendo
        
        bool punto = false; // si ya se ha agregado al número un '.'
        // coordenades del area del path (menor y mayor)
        double xMen = -1; 
        double yMen = -1;
        double xMay = -1; 
        double yMay = -1; 
        
        int pos = 0; // posicion a insertar en aux
        double aux[2]; // guarda el punto encontrado y que se debe procesar
        double temp;
        
        bool movetoQuitado = false; // Indica si ya se quitó el primer mveto del d
        
        int tam = infoPath.size();
        for (int i = 0; i < tam; i++) {
            char c = infoPath[i];
            
            // Pasa los char del número al string
            if((c == '.' && !punto) || (c == '-' && numStr == "") || (int('0') <= int(c) && int(c) <= int('9'))){
                numStr += c;
                if(c == '.')
                    punto = true;
            }
            else{
                // si el char es un código
                if(int('A') <= int(c) && int(c) <= int('Z') || int('a') <= int(c) && int(c) <= int('z')){
                    codigoNuevo = c;
                    // Si no se ha definido un moveto y el código detectado es un moveto
                    if(!movetoQuitado && (codigo == 'M' || codigo == 'm')){
                        
                        this->M_char = codigo;
                        // Quitar moveto del d  (siempre estaría al inicio)
                        this->d = this->d.substr(i, tam - i);
                        movetoQuitado = true;
                    }
                }
                if(numStr != ""){
                    if(codigo == 'M' || codigo == 'm'){
                        // Define la coordenada del moveto
                        if(M_x == -1)
                            M_x = stod(numStr);
                        else if(M_y == -1){
                            M_y = stod(numStr);
                            
                            xMen = M_x;
                            yMen = M_y;
                        }
                    }
                    else{
                        bool cambio = true; // indica si ha ocurrido un combio en alguna coordenada mayor o menor
                        while(cambio && pos == 2){ // continua si ha ocurrido un cambio
                            cambio = false;
                            // Proceso para encontrar el area del path
                            if(xMen == -1 || aux[0] < xMen){
                                temp = xMen;
                                xMen = aux[0];
                                aux[0] = temp;
                                cambio = true;
                            }
                            if(yMen == -1 || aux[1] < yMen){
                                temp = yMen;
                                yMen = aux[1];
                                aux[1] = temp;
                                cambio = true;
                            }
                            if(xMay == -1 || aux[0] > xMay){
                                temp = xMay;
                                xMay = aux[0];
                                aux[0] = temp;
                                cambio = true;
                            }
                            if(yMay == -1 || aux[1] > yMay){
                                temp = yMay;
                                yMay = aux[1];
                                aux[1] = temp;
                                cambio = true;
                            }

                            pos = 0;
                            aux[0] = 0;
                            aux[1] = 0;
                        }
                        // Define el punto en aux
                        aux[pos] = stod(numStr);

                        // Si es una posicion relativa la pasa a absoluta
                        if(int('a') <= int(codigo) && int(codigo) <= int('z')){
                            if(pos == 0)
                                aux[0] += M_x;
                            else
                                aux[1] += M_y;
                        }

                        pos++;

                    }
                    numStr = "";
                    punto = false;
                }
                
                codigo = codigoNuevo;
            }
            
            if(c == '-' && numStr == "")
                numStr += c;
        }
        
        this->M[0] = M_x;
        this->M[1] = M_y;
        
        this->areaInicio[0] = xMen;
        this->areaInicio[1] = yMen;
        this->areaFin[0] = xMay;
        this->areaFin[1] = yMay;
        
        pasarAVector();
    }
    
    void guardarPath(){
        string* pathGuardar = new string();
        
        // Generar nuevo string del path
        string path = this->M_char + to_string(this->M_x) + ',' + to_string(this->M_y);
        
        string dato;
        int cont = 0;
        int tam = this->vectorD.size();
        for (int i = 0; i < tam; i++) {
            dato = this->vectorD[i];
            
            // El dato es el código de instrucción
            if(dato.size() == 1 && ((int('a') <= int(dato[0]) && int(dato[0]) <= int('z')) 
               || (int('A') <= int(dato[0]) && int(dato[0]) <= int('Z')))){
                
                if(path[path.size()-1] == ',') // Quita el ultimo char de ser este una coma
                    path = path.substr(0, path.size()-1);
                
                path += dato;
            }
            
            else // El dato es una coordenada x o y
                path += dato + ',';
            
        }
        pathGuardar[0] = path;
        
        
        // Busca el 'd' y reemplaza su valor por el sting generado
        for (xml_attribute<>* a = nodoXML->first_attribute(); a != NULL; a = a->next_attribute()) {
                string atrNombre = (string)a->name();
                
                for_each(atrNombre.begin(), atrNombre.end(), [](char & c){
                    c = ::tolower(c);
                });
                    
                if(atrNombre == "d")
                    a->value(pathGuardar[0].c_str());
        }
    }
    
    void posicionesBorrarCant(int cant){
        vector<double*>::iterator itPosIni = this->posiciones.begin();
        this->posiciones.erase(itPosIni, itPosIni + cant);
    }
    
    void imprimir(){
        cout << this->id << ";" << this->color << ";" << this->opacidad << ";M" << this->M_x << "," << this->M_y << this->d << endl;
    }
    
    void imprimirPosiciones(){
        double* punto;
        vector<double*>::iterator fin = this->posiciones.end();
        for(vector<double*>::iterator it = this->posiciones.begin(); it != fin; ++it)
        {   
            punto = ((double*)*it);
            cout << punto[0] << ", " << punto[1] << "\n";
        }
        cout << endl;
    }
};




class XML{
private:
    string nombre; // Nombre original del archivo
    file<> xmlFile();
    
    double ancho, alto;
    
    vector<Path*> paths = {};
    
    // puntos de inicio y fin del area
    double* areaInicio = NULL;
    double* areaFin = NULL;
    
    
    vector<Path*> seleccionar(double puntos[][2], int tamP, int colores[][3], int tamC){
        /*
         *  Programación Dinámica
         * 
         *  Al creara el objeto XML se carga la información de cada path en objetos path, 
         *  se define un area general de corte y prepara los paths para la función de ruta: 
         *  Pasa la información del path a un vector. O(n*m)
         * 
         *  Puntos
         *  1. Recorre los puntos indicados excluyendo los que están fuera del area general. O(n)
         *  
         *  Colores
         *  2.1. Recorre los colores introduciéndolos en un array, los combina de haber más de 5. O(n)
         *  2.2. Crea un vector de paths corte según los colores de corte. O(n)
         *  
         *  Selección
         *  3. Recorre los paths de corte validando si su area coincide con un punto de corte,
         *  de ser así lo agrega al vector pathSeleccionados. O(n*m)
         * 
         *      Insertar los paths en un Quadtree por su area podría dar que la selección sea O(nlogn).
         *          - No se como se balancearía o si es posible.
         *          - Si los paths están ubicados a un solo lado del svg el arbol tendría una zona sobrecargada con lo que 
         *          el comportamiento podría ser O(n^2).
         * 
         *      Usar una matriz del tamaño del svg y colocar en cada casilla los paths que se ubiquen allí.
         *          - La implementación que pensé sería O(n^2 * m) aunque buscar la intersección de los puntos
         *          sería O(n).
         */
        
        vector<Path*> pathSeleccionados = {};
        vector<Path*> pathCorte = {};
        vector<double*> puntosCorte = {};
        
        // 1. Recorre los puntos excluyendo los que están fuera del area
        for (int i = 0; i < tamP; i++) {
            double* p = puntos[i];

            // si no está en el area del xml (area de corte)
            if(!isArea(p[0], p[1]))
                continue;

            puntosCorte.push_back(p);
        }
        
        // 2.1. Recorre los colores introduciéndolos en un array, los combina de haber más de 5.
        // Para el color combinado calcula la diferencia promedio entre estos colores.
        auto coloresCorte = new int[5][3];
        int* c; // color seleccionado
        int posColorC = 0; // posición de color corte
        
        int combinados = 0; // Cantidad de colores que se combinaron con el ultimo color (coloresCorte[4])
        int difProm = 0; // Diferencia promedio entre los colores combinados
        for (int i = 0; i < tamC; i++) {
             c = colores[i]; // Selecciona el color
            // Inserta el color
            if(posColorC < 5){
                coloresCorte[posColorC][0] = c[0];
                coloresCorte[posColorC][1] = c[1];
                coloresCorte[posColorC][2] = c[2];
                posColorC++;
            }
            // Si no haber espacio y faltan colores estos los combina con el último insertado
            else{ 
                // diferencia promedio
                int difPromAux = 0;
                difPromAux += abs(colores[i-1][0] - colores[i][0]);
                difPromAux += abs(colores[i-1][1] - colores[i][1]);
                difPromAux += abs(colores[i-1][2] - colores[i][2]);
                // Si difPromAux == 0, el color está repetido
                if(difPromAux != 0){ 
                    coloresCorte[4][0] += c[0]; 
                    coloresCorte[4][1] += c[1]; 
                    coloresCorte[4][2] += c[2]; 
                    
                    difProm += difPromAux;
                    
                    combinados++;
                }
            }
        }
        // Define el color promedio de los colores combinados
        if(combinados > 0){
            if(difProm != 0){
                coloresCorte[4][0] /= combinados + 1;
                coloresCorte[4][1] /= combinados + 1;
                coloresCorte[4][2] /= combinados + 1;
            }
            // diferencia promedio
            difProm /= combinados * 3;
        }
        
        // 2.2. Crea un vector de paths corte según los colores de corte
        int rango = 20;
        
        int* pathColor;
        Path* p;
        int tamPaths = this->paths.size();
        for (int i = 0; i < tamPaths; i++) {
            p = this->paths[i];
            
            c = p->getColorRGB();
            
            // Si difProm > rango, Se estarían incluyendo colores fuera de rango
            
            // posColorC tendría la cantidad de colores en el array
            int rangoConbinados = rango + ((difProm * combinados)/2);
            if(difProm == 0) // difProm == 0, los colores combinados eran iguales
                rangoConbinados = rango;
            
            int r = c[0]; 
            int g = c[1]; 
            int b = c[0]; 
            if(posColorC >= 1
               && coloresCorte[0][0] - rango <= r && r <= coloresCorte[0][0] + rango
               && coloresCorte[0][1] - rango <= g && g <= coloresCorte[0][1] + rango
               && coloresCorte[0][2] - rango <= b && b <= coloresCorte[0][2] + rango){
                
                pathCorte.push_back(p);
            }
            else if(posColorC >= 2
               && coloresCorte[1][0] - rango <= r && r <= coloresCorte[1][0] + rango
               && coloresCorte[1][1] - rango <= g && g <= coloresCorte[1][1] + rango
               && coloresCorte[1][2] - rango <= b && b <= coloresCorte[1][2] + rango){
                
                pathCorte.push_back(p);
            }
            else if(posColorC >= 3
               && coloresCorte[2][0] - rango <= r && r <= coloresCorte[2][0] + rango
               && coloresCorte[2][1] - rango <= g && g <= coloresCorte[2][1] + rango
               && coloresCorte[2][2] - rango <= b && b <= coloresCorte[2][2] + rango){
                
                pathCorte.push_back(p);
            }
            else if(posColorC >= 4
               && coloresCorte[3][0] - rango <= r && r <= coloresCorte[3][0] + rango
               && coloresCorte[3][1] - rango <= g && g <= coloresCorte[3][1] + rango
               && coloresCorte[3][2] - rango <= b && b <= coloresCorte[3][2] + rango){
                
                pathCorte.push_back(p);
            }
            else if(posColorC == 5
               && coloresCorte[4][0] - rangoConbinados <= r && r <= coloresCorte[4][0] + rangoConbinados
               && coloresCorte[4][1] - rangoConbinados <= g && g <= coloresCorte[4][1] + rangoConbinados
               && coloresCorte[4][2] - rangoConbinados <= b && b <= coloresCorte[4][2] + rangoConbinados){
                
                pathCorte.push_back(p);
            }
        }
        
        // 3. Recorre los paths de corte y los puntos de corte validando si coinciden
        double* punto;
        tamPaths = pathCorte.size();
        int tamPuntosC = puntosCorte.size();
        for (int i = 0; i < tamPaths; i++){   
            p = pathCorte[i];
            
            for (int j = 0; j < tamPuntosC; j++) {
                punto = puntosCorte[j];
                if(p->isArea(punto[0], punto[1])){ // si el punto está en el area del path
                    pathSeleccionados.push_back(p);
                    break;
                }
            }
        }
        return pathSeleccionados;
    }
    
    void ruta(vector<Path*> pathSeleccionados, double angulo, int frames, bool curva){
        /*  
         *  Recorre los paths seleccionados y para cada path calcula la ruta
         *  de ser necesario. De ser un angulo recto incrementa el 'x' o 'y'.
         *  Luego guarda todos los puntos calculados en el objeto path.
         * 
         *  El incremento base es 1 en 'x' o 'y'
         * 
         */
        double* posAct;
        double xSig;
        double ySig;
        
        // marca si es un angulo negativo
        bool negativo = false;
        if(angulo < 0)
            negativo = true;
        
        angulo = abs(angulo);
        
        // Pasa el angulo a uno equivalente < 360
        double aux;
        if(angulo > 360){
            aux = ((double)(angulo/360)); // porcentaje
            angulo = (aux - ((int)aux)) * 360; // le quita la parte entera y define el angulo equivalente
        }
        
        // Pasa el angulo negativo a uno equivalente positivo
        if(negativo)
            angulo = 360 - angulo;
        
        if(angulo == 0 || angulo == 90 || angulo == 180 || angulo == 270)
            angulo++;
        else if(angulo == 360)
            angulo--;
        
        
        // define el angulo en radianes
        double radianes = (angulo / 180) * M_PI;
        
        double x;
        double y;
        
        double xDif;
        double yDif;
        
        Path* p;
        int tamPath = pathSeleccionados.size();
        for(int i = 0; i < tamPath; i++) // Recorre los paths
        {   
            p = pathSeleccionados[i];
            posAct = p->getPosMoveto();
            
            // Proceso para definir la posición final
            // Si está en el I o IV Cuadrante
            if((90 > angulo && angulo > 0) || (360 > angulo && angulo > 270)){
                xDif = ancho - posAct[0];
                yDif = tan(radianes) * xDif * -1;
                x = ancho;
            }
            else{
                xDif = posAct[0];
                yDif = tan(radianes) * xDif;
                x = 0;
            }
            y = posAct[1] + yDif;

            // Si el 'Y' sobrepasó la imagen
            if(y < 0 || y > alto){
                if(y < 0){
                    yDif = posAct[1];
                    y = 0;
                }
                else if(y > alto){
                    yDif = alto - posAct[1];
                    y = alto;
                }
                xDif = abs(yDif / tan(radianes));

                // Si está en el I o IV Cuadrante
                if((90 > angulo && angulo > 0) || (360 > angulo && angulo > 270))
                    x = posAct[0] + xDif;
                else
                    x = posAct[0] - xDif;
            }

            // (x, y) Es la posición final
            xSig = posAct[0];
            ySig = posAct[1];
            double* rectaInf = recta(xSig, ySig, x, y); // retorna {m, b}
            double m = rectaInf[0];
            double b = rectaInf[1];

            xDif = x - xSig;
            yDif = y - ySig;
            if(curva){
                // Recta Tangente
                double mT = -1 / m; // m*mT = -1

                // Proceso para hayar el punto central de la recta normal
                if(abs(xDif) > abs(yDif)){
                    xSig += xDif / 2;
                    ySig = (m * xSig) + b;
                }
                else{
                    ySig += yDif / 2;
                    xSig = (ySig - b) / m;
                }

                // (xSig, ySig) punto central
                // b = y - m*x
                double bT = ySig - (mT * xSig); // 'b' de la rectga tangente

                // Hayar el punto medio para la curva (p2) de bezierCurve
                if(abs(xDif) > abs(yDif)){
                    if((int)xDif % 2) // Es impar
                        ySig = 2;
                    else
                        ySig = this->alto-2;
                    xSig = (ySig - bT) / mT;
                }
                else{
                    if((int)yDif % 2) // Es impar
                        xSig = 2;
                    else
                        xSig = this->ancho-2;
                    ySig = (mT * xSig) + bT;
                }
                
                if(xSig < 0)
                    xSig = 2;
                else if(xSig > this->ancho)
                    xSig = this->ancho-2;
                if(ySig < 0)
                    ySig = 2;
                else if(ySig > this->alto)
                    ySig = this->alto-2;
                
                // Puntos de la curva
                double** puntos = bezierCurve(posAct[0], posAct[1], xSig, ySig, x, y, frames);
                for (int i = 0; i < frames; i++)
                    p->addPosicion(puntos[i]);
            }
            else{
                // Proceso para hayar los puntos de la recta
                // Define los puntos según x
                if(abs(xDif) > abs(yDif)){ 
                    double incremento = xDif / frames;

                    for (int i = 0; i < frames; i++){
                        xSig += incremento;
                        ySig = (m * xSig) + b;

                        double* punto = new double(2);
                        punto[0] = xSig;
                        punto[1] = ySig;
                        p->addPosicion(punto);
                    }
                }
                else{// Define los puntos según y
                    double incremento = yDif / frames;

                    for (int i = 0; i < frames; i++){
                        ySig += incremento;
                        xSig = (ySig - b) / m;

                        double* punto = new double(2);
                        punto[0] = xSig;
                        punto[1] = ySig;
                        p->addPosicion(punto);
                    }
                }
            }
        }
        
    }
    
    void guardarFrame(int frameNum){
        string nuevoNombre = this->nombre + to_string(frameNum) + ".svg";
        
        stringstream ss;
        ss << *raiz.first_node(); // Pasa el nodo raiz al stringstream (ss)
        string stringXML = ss.str(); // Pasa a string el xml
        
        ofstream file(nuevoNombre.c_str());
        file << stringXML; // Guarda el stringXML en el nuevo archivo
        file.close();
    }
    
    void frame(vector<Path*> pathSeleccionados, int frames){
        /*  Voraz
         * 
         *  Backtracking - poda
         * 
         *  Las fases serían los frames a generar
         *  
         *  Óptimo: Posición (punto) a mover el path
         *  Criterio: Punto que concuerde con la cantidad de puntos
         *  dividido entre los frames
         */
        
        // Descarta los paths que apenas se moverían
        Path* p;
        int tamPath = pathSeleccionados.size();
        
        for (int i = 0; i < tamPath; i++) // Recorre los paths
        {   
            p = pathSeleccionados[i]; // Path
            
            // Borra el path Si la cantidad de puntos es menor al 25% de los frames
            if(p->getPosTam() < frames * 0.25){
                pathSeleccionados.erase(pathSeleccionados.begin() + i);
                i--;
            }
        }
        
        tamPath = pathSeleccionados.size();
        for (int etapa = 1; etapa <= frames; etapa++) { // Recorre los paths para cada frame
            for (int i = 0; i < tamPath; i++) // Recorre los paths
            {  
                p = pathSeleccionados[i]; // Path
                
                double* punto = ((double*)*(p->getPosiciones().begin()));
                double* puntoMover = new double[2];
                puntoMover[0] = punto[0];
                puntoMover[1] = punto[1];
                
                p->posicionesBorrarCant(1);
                
                p->mover(puntoMover);
                p->guardarPath();
            }
            // Se ha actualizado las posiciones de los paths
            // Guarda el xml como nuevo frame
            guardarFrame(etapa);
        }
    }
    
    void imprimir(xml_node<>* nodo){
        // Imprime etiquetas y sus valores
        cout << "Etiqueta: " << nodo->name() << endl;
        for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
            cout << "\tAtributo: " << a->name();
            cout << "\tValor: " << a->value();
        }
        
        // Recorre el primer hijo
        xml_node<>* nodoNuevo = nodo->first_node(); 
        if(nodoNuevo != NULL)
            imprimir(nodoNuevo);
        // Si no tiene primer hijo recorre a su siguiente hermano
        nodoNuevo = nodo->next_sibling(); 
        if(nodoNuevo != NULL)
            imprimir(nodoNuevo);
    }
    
    /*
     * Lee el xml para definir el ancho y altura
     */
    bool setTam(xml_node<>* nodo){
        if((string)nodo->name() == "svg"){
            for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
                if((string)a->name() == "viewBox"){
                    string valores[4] = {};
                    //split
                    stringstream ss(a->value());
                    string v;
                    int i = 0;
                    while (ss >> v) {
                        valores[i] = v;
                        i++;
                    }

                    this->ancho = strtod(valores[2].c_str(), NULL);
                    this->alto = strtod(valores[3].c_str(), NULL);
                    
                    return true;
                }
            }
        }
        
        // Recorre el primer hijo
        xml_node<>* nodoNuevo = nodo->first_node(); 
        if(nodoNuevo != NULL && setTam(nodoNuevo))
            return true;
        // Si no tiene primer hijo recorre a su siguiente hermano
        nodoNuevo = nodo->next_sibling(); 
        if(nodoNuevo != NULL && setTam(nodoNuevo))
            return true;
        
        return false;
    }
    
    void agregarPath(Path* p){
        this->paths.push_back(p);
        
        double* areaIniPath = p->getAreaInicio();
        double* areaFinPath = p->getAreaFin();
        
        // Se han definido las areas
        if(this->areaInicio != NULL){
            if(areaIniPath[0] < this->areaInicio[0])
                this->areaInicio[0] = areaIniPath[0];
            if(areaIniPath[1] < this->areaInicio[1])
                this->areaInicio[1] = areaIniPath[1];
            
            if(areaFinPath[0] > this->areaFin[0])
                this->areaFin[0] = areaFinPath[0];
            if(areaFinPath[1] > this->areaFin[1])
                this->areaFin[1] = areaFinPath[1];
        }
        else{ // No se ha definido el area
            this->areaInicio = areaIniPath;
            this->areaFin = areaFinPath;
        }
    }
   
    void iniciarVecPaths_aux(xml_node<>* nodo){
        if((string)nodo->name() == "path"){
            Path* p = new Path();
            for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
                string atrNombre = (string)a->name();
                
                for_each(atrNombre.begin(), atrNombre.end(), [](char & c){
                    c = ::tolower(c);
                });
                    
                p->setNodoXML(nodo);
                
                if(atrNombre == "id")
                    p->setId(a->value());
                else if(atrNombre == "opacity")
                    p->setOpacidad(a->value());
                else if(atrNombre == "d")
                    p->path(a->value());
                else if(atrNombre == "fill")
                    p->setColor(a->value());
                else if(atrNombre == "stroke")
                    p->setColor(a->value());
            }
            agregarPath(p);
        }
        
        // Recorre el primer hijo
        xml_node<>* nodoNuevo = nodo->first_node();
        if(nodoNuevo != NULL)
            iniciarVecPaths_aux(nodoNuevo);
        
        // Si no tiene primer hijo recorre a su siguiente hermano
        nodoNuevo = nodo->next_sibling(); 
        if(nodoNuevo != NULL)
            iniciarVecPaths_aux(nodoNuevo);
    }
    
    /*
     * Inicia el array de paths
     */
    void iniciarVecPaths(){
        iniciarVecPaths_aux(raiz.first_node());
    }
    
    /*
     * Pone en 0s el array entrante
     */
    void limpiarNumArray(double *arr, int tam){
        for (int i = 0; i < tam; i++)
            arr[i] = 0;
    }
    
    bool isArea(double x, double y){
        return x >= this->areaInicio[0] && y >= this->areaInicio[1] && x <= this->areaFin[0] && y <= this->areaFin[1];
    }
    
public:
    XML(string dir){
        // Pasa el string a char*
        this->nombre = dir.substr(0, dir.length()-4); // Guarda el nombre original del archivo
        
        setTam(raiz.first_node());
        
        iniciarVecPaths();
    }
    
    
    void animacion(double puntos[][2], int tamP, int colores[][3], int tamC, double angulo, int frames, bool curva){
        if (frames > 500) // Limite de frames
            frames = 500;
        
        vector<Path*> pathSeleccionados = seleccionar(puntos, tamP, colores, tamC);
        
        ruta(pathSeleccionados, angulo, frames, curva);
        frame(pathSeleccionados, frames);
        
    }
    
    void imprimir(){
        xml_node<>* nodo = raiz.first_node();
        imprimir(nodo);
    }
    
    /*
     *  Imprime el Ancho y el Alto del xml(svg)
     */
    void imprimirTam(){
        cout << "Ancho: " << this->ancho;
        cout << "\tAncho: " << this->alto << endl;
    }
    
    void imprimirPaths(){
        vector<Path*>::iterator fin = this->paths.end();
        for(vector<Path*>::iterator it = this->paths.begin(); it != fin; ++it)
        {
            ((Path*)*it)->imprimir();
        }
    }
    
    void imprimirArea(){
        cout << this->areaInicio[0] << "," << this->areaInicio[1] 
             << ";" << this->areaFin[0] << "," << this->areaFin[1] << endl;
    }
    
    void imprimirXML(){
        cout << *raiz.first_node() << endl;
    }
};

/*
 * 
 */
int main(int argc, char** argv) {
    string nombre = "wifi.svg";
    char* arr = strToChar(nombre);
    file<> file(arr);
    
    raiz.parse<0>(file.data()); // Parsea el xml a un arbol DOM
    
    XML* archivoXML = new XML(nombre);
    
   
    double puntos[][2] = {{0,0},{100,100},{1000,1000},{300,350},{1087.849,614.77},{3000,3500}};
    int tamP = (sizeof(puntos) / sizeof(puntos[0]));
    
    int colores[][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,255,255},{0,0,0}};
    int tamC = (sizeof(colores) / sizeof(colores[0]));
    
    double angulo = 0;
    int frames = 32;
    
    bool curva = false;
    
    archivoXML->animacion(puntos, tamP, colores, tamC, angulo, frames, curva);
    
    return 0;
}

