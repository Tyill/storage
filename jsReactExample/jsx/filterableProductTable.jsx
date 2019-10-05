/* eslint-disable no-unused-vars */

import React from "react"

import {SearchBar} from "./searchBar.jsx"
import {ProductTable} from "./productTable.jsx"

export {FilterableProductTable}


class FilterableProductTable extends React.Component {

  constructor(props){
    super(props);   

    this.state = {filterText: '', inStockOnly: false};
    this.handleInStockOnlyChange = this.handleInStockOnlyChange.bind(this);
    this.handleFilterTextChange = this.handleFilterTextChange.bind(this);
  }

  handleInStockOnlyChange(e){

    this.setState(state => {
      return { inStockOnly : !state.inStockOnly};
    });
  }

  handleFilterTextChange(e){

    this.setState({ filterText : e.target.value});
  }

  render(){ 
   
    return (
        <div>
         <SearchBar {...this.props} 
           filterText = {this.state.filterText}
           inStockOnly = {this.state.inStockOnly}
           handleInStockOnlyChange = {this.handleInStockOnlyChange}
           handleFilterTextChange = {this.handleFilterTextChange} />
         <ProductTable {...this.props} 
           filterText = {this.state.filterText}
           inStockOnly = {this.state.inStockOnly} />
        </div>
    );
  }
}