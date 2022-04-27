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
    
    string color = "#000000";
    
    string id = "";
    string opacidad = "";
    
    string d = "";
public:
    void setNodoXML(xml_node<>* nodoXML) {
        this->nodoXML = nodoXML;
    }

    void setColor(string color) {
        this->color = color;
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

    
    string getColor() const {
        return color;
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
     * Analiza y define el area según la información del path entrante
     */
    void path(string infoPath){
        if(toupper(infoPath[infoPath.size()-1]) != 'Z')
            infoPath += 'z'; // Agrega 'z' para marcar el final del path
        
        this->d = infoPath;
        
        //coordenada indicada por moveto
        double M_x = -1;
        double M_y = -1;
        
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
        for (char c : infoPath) {
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
                    
                    if(codigo != ' ' && codigoNuevo == 'M'){
                        M_x = -1;
                        M_y = -1;
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
        cout << this->id << ";" << this->color << ";" << this->opacidad << ";" << this->areaInicio[0] 
             << "," << this->areaInicio[1] << ";" << this->areaFin[0] << "," << this->areaFin[1] << endl;
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
    
    vector<Path*> seleccionar(double puntos[][2], int tamP, string* colores, int tamC){
        // Programación Dinámica
        
        vector<Path*> pathSeleccionados = {};
        vector<double*> puntosCorte = {};
        
        // Recorre los puntos excluyendo los que están fuera del area
        for (int i = 0; i < tamP; i++) {
            double* p = puntos[i];

            // si no está en el area del xml (area de corte)
            if(!isArea(p[0], p[1]))
                continue;

            puntosCorte.push_back(p);
        }
        
        // pasa los colores a un unordered_map
        unordered_map<string, int> mapColores;
        for (int i = 0; i < tamC; i++) {
            string c = colores[i];
            mapColores[c] = 1;
        }
        
        Path* p;
        double* punto;
        vector<Path*>::iterator fin = this->paths.end();
        vector<double*>::iterator finPuntos = puntosCorte.end();
        for(vector<Path*>::iterator it = this->paths.begin(); it != fin; ++it) // Recorre los paths
        {   
            p = ((Path*)*it);
            string pathColor = p->getColor(); // selecciona el color del path
            
            // Si el path es de un color indicado
            if (mapColores.find(pathColor) != mapColores.end())
                for(vector<double*>::iterator itPuntos = puntosCorte.begin(); itPuntos != finPuntos; ++itPuntos) // Recorre los puntos
                {   
                    punto = ((double*)*itPuntos);
                    if(p->isArea(punto[0], punto[1])){ // si el punto está en el area del path
                        pathSeleccionados.push_back(p);
                        break;
                    }
                }
        }
        
        return pathSeleccionados;
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
    string colores[] = {"#000000"};
    int tamC = (sizeof(colores) / sizeof(colores[0]));
    
    vector<Path*> pathSeleccionados = archivoXML->seleccionar(puntos, tamP, colores, tamC);
    
    Path* p;
    vector<Path*>::iterator fin = pathSeleccionados.end();
    for(vector<Path*>::iterator it = pathSeleccionados.begin(); it != fin; ++it)
    {   
        p = ((Path*)*it);
        p->imprimir();
    }
    
    return 0;
}

