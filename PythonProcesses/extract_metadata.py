# -*- coding: utf-8 -*-
"""
Created on Mon May  3 18:53:09 2021

@author: saulr
"""
import json

papers = []
idx = 1
with open("arxiv-metadata-oai-snapshot.json", 'r') as f:
    for paper in f:
        newpaper = json.loads(paper)
        newpaper = {
                'id': newpaper['id'],
                'title': newpaper['title'],
                'abstract': newpaper['abstract'],
            }
        if idx > 5:
            break
        idx+=1
        papers.append(newpaper)                    

with open('extracted_metadata_aux.json', 'w') as outfile:
    json.dump(papers, outfile)
    
del papers


   
        
        
        
        
        
        