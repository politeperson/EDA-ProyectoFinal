# -*- coding: utf-8 -*-
"""
Created on Mon May 17 09:11:06 2021

@author: saulr
"""
import json
import re
import string
import pandas as pd

stop_words_csv = pd.read_csv("stop_words_list.csv").to_dict()

# removing spaces from keys
# storing them in sam dictionary
stop_words_csv = { x.translate({32:None}) : y for x, y in stop_words_csv.items()}

# definiré el alfabeto para eliminar aquellos caracteres que no estén presentes
# en cada texto
alphabet = {
    # 26 letras del alfabeto inglés
    'a': True,
    'b': True,
    'c': True,
    'd': True,
    'e': True,
    'f': True,
    'g': True,
    'h': True,
    'i': True,
    'j': True,
    'k': True,
    'l': True,
    'm': True,
    'n': True,
    'o': True,
    'p': True,
    'q': True,
    'r': True,
    's': True,
    't': True,
    'u': True,
    'v': True,
    'w': True,
    'x': True,
    'y': True,
    'z': True,
    # números
    '0': True,
    '1': True,
    '2': True,
    '3': True,
    '4': True,
    '5': True,
    '6': True,
    '7': True,
    '8': True,
    '9': True,
    # caracteres especiales
    '$': True,
    '-': True,
}

def format_data(title : str, abstract : str):
    # obtendremos una lista de palabras al separarlas por un espacio
    list_of_words = (title + abstract).lower().split(" ")
    transformed_data = ""
    punct = string.punctuation
    punct = punct.replace("-", "") # aceptaremos el signo '-'
    
    for word in list_of_words:
        # primero reemplazamos todos los saltos de línea por vacío
        transformed_word = word.translate(str.maketrans('', '', '\n'))
        transformed_word = re.sub("\$[^\$]*\$", "", transformed_word)
        transformed_word = transformed_word.translate(str.maketrans('', '', punct))
        
        
        if transformed_word == "" or transformed_word in stop_words_csv.keys():
            continue
        
        for letter in transformed_word:
            if letter not in alphabet:
                transformed_word = transformed_word.translate({ord(letter): None})
        
        if transformed_word == "" or transformed_word in stop_words_csv.keys():
            continue
        
        transformed_data += transformed_word
    
    
    return transformed_data


read_metadata = open('../python_processes/extracted_metadata.json', 'r')
formatted_metadata = open('formatted_metadata.txt', 'w', encoding="utf-8")

loaded_papers = json.load(read_metadata)
for paper in loaded_papers:
    id_paper = paper['id'] 
    
    transformed_data = format_data(paper['title'], paper['abstract'])
    formatted_metadata.write("{} {}\n".format(id_paper, transformed_data))
    
    
read_metadata.close()
formatted_metadata.close()
