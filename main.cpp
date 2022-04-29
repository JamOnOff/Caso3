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

using namespace std;
using namespace rapidxml;


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
    
    string d = "";
    
public:
    
    vector<double*> posiciones;
    
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
    
    string getId() const {
        return id;
    }

    string getOpacidad() const {
        return opacidad;
    }
    
    double* getPosMoveto() const {
        double M[2] = {M_x, M_y};
        double* Mpuntero = M;
        return Mpuntero;
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
     * Pasa a posición relativa las coordenadas del path
     */
    void pasarRelativo(){
        string nuevoPath = "";
        
        string numStr = ""; // donde se gurdan los char del numero a pasar a double
        char codigo = ' '; // codigo el cual se está recorriendo
        
        bool punto = false; // si ya se ha agregado al número un '.'
        
        int pos = 0; // posicion a insertar en aux
        double aux[2]; // guarda el punto encontrado y que se debe procesar
        
        bool posAbs = false; // indica si es una posicion absoluta
        
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
                // si el char es un código
                if(int('A') <= int(c) && int(c) <= int('Z')){
                    posAbs = true;
                    
                    // pasa codigo a relativo (minuscula)
                    codigo = char(int('a') + int(c) - int('A'));
                    
                    nuevoPath += codigo;
                }
                else if(int('a') <= int(c) && int(c) <= int('z')){
                    posAbs = false;
                    codigo = c;
                    
                    if(c != 'z')
                        nuevoPath += c;
                }
                
                if(numStr != ""){
                    // Define el punto en aux
                    aux[pos] = stod(numStr);

                    pos++;
                    
                    if(pos == 2){
                        // Pasa la coordenada en aux a relativo
                        if(posAbs){
                            aux[0] -= this->M_x;
                            aux[1] -= this->M_y;
                        }
                        if(int('0') <= int(nuevoPath[nuevoPath.size()-1]) && int(nuevoPath[nuevoPath.size()-1]) <= int('9'))
                            nuevoPath += ',';
                        
                        nuevoPath += to_string(aux[0]) + ',' + to_string(aux[1]);
                        
                        pos = 0;
                        aux[0] = 0;
                        aux[1] = 0;
                    }
                    
                    numStr = "";
                    punto = false;
                }
            }
            
            if(c == '-' && numStr == "")
                numStr += c;
        }
        
        nuevoPath += 'z';
        this->d = nuevoPath;
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
                    if(codigo == 'M'){
                        // Quitar moveto del d
                        this->d = this->d.substr(i, tam - i);
                    }
                }
                if(numStr != ""){
                    if(codigo == 'M'){
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
    
        this->areaInicio[0] = xMen;
        this->areaInicio[1] = yMen;
        this->areaFin[0] = xMay;
        this->areaFin[1] = yMay;
    }
    
    
    
    void imprimir(){
        cout << this->id << ";" << this->color << ";" << this->opacidad << ";M" << this->M_x << "," << this->M_y << this->d << endl;
    }
};

class XML{
private:
    xml_document<> raiz; // Arbol DOM
    double ancho, alto;
    
    vector<Path*> paths = {};
    
    // puntos de inicio y fin del area
    double* areaInicio = NULL;
    double* areaFin = NULL;
    
    void imprimir(xml_node<>* nodo){
        // Imprime etiquetas y sus valores
        cout << "Etiqueta: " << nodo->name() << endl;
        for (xml_attribute<>* a = nodo->first_attribute(); a != NULL; a = a->next_attribute()) {
            cout << "\tAtributo: " << a->name() << endl;
            cout << "\t\tValor: " << a->value() << endl;
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
        iniciarVecPaths_aux(this->raiz.first_node());
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
        char arr[dir.length() + 1]; 
        strcpy(arr, dir.c_str()); 
        
        file<> file(arr);           // Lee y carga el archivo en memoria
        raiz.parse<0>(file.data()); // Parsea el xml a un arbol DOM
        
        setTam(raiz.first_node());
        
        iniciarVecPaths();
    }
    
    vector<Path*> seleccionar(double puntos[][2], int tamP, int colores[][3], int tamC){
        // Programación Dinámica
        
        vector<Path*> pathSeleccionados = {};
        vector<Path*> pathCorte = {};
        vector<double*> puntosCorte = {};
        
        // Recorre los puntos excluyendo los que están fuera del area
        for (int i = 0; i < tamP; i++) {
            double* p = puntos[i];

            // si no está en el area del xml (area de corte)
            if(!isArea(p[0], p[1]))
                continue;

            puntosCorte.push_back(p);
        }
        
        // Recorre los paths excluyendo los que no tengan un color cercano a uno indicado.
        int rango = 15;
        int* c;
        int* pathColor;
        Path* p;
        vector<Path*>::iterator fin = this->paths.end();
        for(vector<Path*>::iterator it = this->paths.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            pathColor = p->getColorRGB(); // selecciona el color rgb del path
            
            for (int i = 0; i < tamC; i++) {
                c = colores[i];
                
                if(c[0] - rango <= pathColor[0] && c[1] - rango <= pathColor[1] && c[2] - rango <= pathColor[2]
                   && c[0] + rango >= pathColor[0] && c[1] + rango >= pathColor[1] && c[2] + rango >= pathColor[2]){
                    pathCorte.push_back(p);
                    break;
                }
            }
        }
        
        // Recorre los paths de corte y los puntos de corte validando si coinciden
        double* punto;
        fin = pathCorte.end();
        vector<double*>::iterator finPuntos = puntosCorte.end();
        for(vector<Path*>::iterator it = pathCorte.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            
            for(vector<double*>::iterator itPuntos = puntosCorte.begin(); itPuntos != finPuntos; ++itPuntos) // Recorre los puntos
            {   
                punto = ((double*)*itPuntos);
                if(p->isArea(punto[0], punto[1])){ // si el punto está en el area del path
                    pathSeleccionados.push_back(p);
                    break;
                }
            }
        }
        
        // Pasa a posición relativa las coordenadas de los paths seleccionados
        fin = pathSeleccionados.end();
        for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            p->pasarRelativo();
        }
        
        return pathSeleccionados;
    }
    
    void ruta(vector<Path*> pathSeleccionados, double angulo){
        double* posAct;
        double xSig;
        double ySig;
        
        bool negativo = false;
        if(angulo < 0)
            negativo = true;
        
        angulo = abs(angulo);
        
        double aux;
        if(angulo > 360){
            aux = ((double)(angulo/360)); // porcentaje
            angulo = (aux - ((int)aux)) * 360; // le quita la parte entera y define el angulo equivalente
        }
        
        if(negativo)
            angulo = 360 - angulo;
        
        double radianes = (angulo / 180) * M_PI;
        
        double x;
        double y;
        
        Path* p;
        vector<Path*>::iterator fin = pathSeleccionados.end();
        for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it)
        {   
            p = ((Path*)*it);
            posAct = p->getPosMoveto();
            
            if(angulo != 0 && angulo != 90 && angulo != 180 && angulo != 270 && angulo != 360){
                double xDif;
                double yDif;
                // si está en el I o IV Cuadrante
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

                // si el 'Y' sobrepasó la imagen
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

                    // si está en el I o IV Cuadrante
                    if((90 > angulo && angulo > 0) || (360 > angulo && angulo > 270))
                        x = posAct[0] + xDif;
                    else
                        x = posAct[0] - xDif;
                }
                
                // (x, y) es la posición final
                
            }
            else{
                xSig = posAct[0];
                ySig = posAct[1];
                // mientras la posición no esté fuera de la imagen
                while(0 < xSig && xSig < this->ancho && 
                      0 < ySig && ySig < this->alto){

                    if(angulo == 0 || angulo == 360)
                        xSig++;
                    else if(angulo == 180)
                        xSig--;
                    else if(angulo == 270)
                        ySig++;
                    else if(angulo == 90)
                        ySig--;
                    
                    double posSig[2] = {xSig, ySig};
                    double* posSigPuntero = posSig;
                    p->posiciones.push_back(posSigPuntero);
                }
            }
            
        }
    }
    
    void animacion(double puntos[][2], int tamP, int colores[][3], int tamC, double angulo){
        vector<Path*> pathSeleccionados = seleccionar(puntos, tamP, colores, tamC);
        ruta(pathSeleccionados, angulo);
        /*
        Path* p;
        vector<Path*>::iterator fin = pathSeleccionados.end();
        for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it)
        {   
            p = ((Path*)*it);
            p->imprimir();
        }
        */
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
};

/*
 * 
 */
int main(int argc, char** argv) {
    XML* archivoXML = new XML("wifi-1.svg");
    
    double puntos[][2] = {{0,0},{100,100}};
    int tamP = (sizeof(puntos) / sizeof(puntos[0]));
    int colores[][3] = {{0,0,0}};
    int tamC = (sizeof(colores) / sizeof(colores[0]));
    
    double angulo = -35;
    
    archivoXML->animacion(puntos, tamP, colores, tamC, angulo);
    
    return 0;
}

