// eslint-disable-next-line no-unused-vars
import React from "react";
import PropTypes from 'prop-types';

export {ProductRow}

function ProductRow(props){
    if (props.stocked)
      return <tr><td className="text-danger">{props.name}</td><td>{props.price}</td></tr>;
    else
      return <tr><td>{props.name}</td><td>{props.price}</td></tr>;
} 

ProductRow.propTypes = {   
    name: PropTypes.string,
    price: PropTypes.string,
  };