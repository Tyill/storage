/* eslint-disable no-unused-vars */
import React from "react";
import ReactDOM from "react-dom";
import PropTypes from 'prop-types';
import "../node_modules/bootstrap/dist/css/bootstrap.min.css";
import {FilterableProductTable} from "./filterableProductTable.jsx"
 
function App() {

    let prods = [
      {category: "Sporting Goods", price: "$49.99", stocked: true, name: "Football"},
      {category: "Sporting Goods", price: "$9.99", stocked: true, name: "Baseball"},
      {category: "Sporting Goods", price: "$29.99", stocked: false, name: "Basketball"},
      {category: "Electronics", price: "$99.99", stocked: true, name: "iPod Touch"},
      {category: "Electronics", price: "$399.99", stocked: false, name: "iPhone 5"},
      {category: "Electronics", price: "$199.99", stocked: true, name: "Nexus 7"}
    ];

    return <FilterableProductTable prods = {prods}/>
}

ReactDOM.render(
  <App />, 
  document.getElementById('root')
);

App.propTypes = {
  category: PropTypes.string,
  name: PropTypes.string,
  price: PropTypes.string,
  stocked: PropTypes.bool,
};