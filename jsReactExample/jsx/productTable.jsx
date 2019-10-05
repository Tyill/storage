/* eslint-disable no-unused-vars */
import React from "react";
import PropTypes from 'prop-types';
import {ProductRow} from "./productRow.jsx";
import {ProductCategoryRow} from "./productCategoryRow.jsx";


export {ProductTable}

class ProductTable extends React.Component {

    constructor(props){
      super(props);      
    }

    render(){
      
      this.props.prods.sort((a, b) =>{
        a.category < b.category;
      });

      let prods = this.props.prods;

      if (this.props.inStockOnly)
        prods = this.props.prods.filter(item => item.stocked);

      if (this.props.filterText.length > 0)
        prods = prods.filter(item => item.name.startsWith(this.props.filterText));
      
      let categoryMem = ""; 
      return <table> 
              <tbody>
               <tr>
                <th><b>Name</b></th>
                <th><b>Price</b></th>
               </tr>              
               {                
                prods.map(item =>{
                  if (categoryMem != item.category){
                    categoryMem = item.category;                    
                    return [
                        <ProductCategoryRow key = {item.category} category = {item.category}></ProductCategoryRow>,
                        <ProductRow key = {item.name} name = {item.name} price = {item.price} stocked = {item.stocked}></ProductRow>
                    ]
                  }
                  else{
                    return <ProductRow key = {item.name} name = {item.name} price = {item.price} stocked = {item.stocked}></ProductRow>
                  }
                })
               }
              </tbody>
             </table>;
    }
}

ProductTable.propTypes = {
  category: PropTypes.string,
  name: PropTypes.string,
  price: PropTypes.string,
  stocked: PropTypes.bool,
};