// eslint-disable-next-line no-unused-vars
import React from "react";
import PropTypes from 'prop-types';

export {ProductCategoryRow}

function ProductCategoryRow(props){

    return <tr><td><b>{props.category}</b></td></tr>;
}

ProductCategoryRow.propTypes = {
    category: PropTypes.string
};