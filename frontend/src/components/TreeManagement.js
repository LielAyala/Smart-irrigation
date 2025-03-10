import React, { useState, useEffect } from 'react';

const TreeManagement = () => {
    const [trees, setTrees] = useState([]);
    const [plants, setPlants] = useState([]);
    const [newPlantName, setNewPlantName] = useState('');
    const [newTreePlantName, setNewTreePlantName] = useState('');
    const [updatePlantOldName, setUpdatePlantOldName] = useState('');
    const [updatePlantNewName, setUpdatePlantNewName] = useState('');
    const [deleteTreeName, setDeleteTreeName] = useState('');
    const [deletePlantName, setDeletePlantName] = useState('');

    useEffect(() => {
        fetch('http://localhost:3011/tree/tree/all')
            .then(res => res.json())
            .then(data => setTrees(data))
            .catch(err => console.error('Error fetching trees:', err));

        fetch('http://localhost:3011/tree/plant/all')
            .then(res => res.json())
            .then(data => setPlants(data))
            .catch(err => console.error('Error fetching plants:', err));
    }, []);

    const addPlant = () => {
        fetch('http://localhost:3011/tree/plant/add', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name: newPlantName })
        })
        .then(() => window.location.reload())
        .catch(err => console.error('Error adding plant:', err));
    };

    const addTree = () => {
        fetch('http://localhost:3011/tree/tree/add', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ plantName: newTreePlantName })
        })
        .then(() => window.location.reload())
        .catch(err => console.error('Error adding tree:', err));
    };

    const updatePlant = () => {
        fetch('http://localhost:3011/tree/plant/update', {
            method: 'PATCH',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ oldName: updatePlantOldName, newName: updatePlantNewName })
        })
        .then(() => window.location.reload())
        .catch(err => console.error('Error updating plant:', err));
    };

    const deleteTree = () => {
        fetch('http://localhost:3011/tree/tree/delete', {
            method: 'DELETE',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name: deleteTreeName })
        })
        .then(() => window.location.reload())
        .catch(err => console.error('Error deleting tree:', err));
    };

    const deletePlant = () => {
        fetch('http://localhost:3011/tree/plant/delete', {
            method: 'DELETE',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name: deletePlantName })
        })
        .then(() => window.location.reload())
        .catch(err => console.error('Error deleting plant:', err));
    };

    return (
        <div className="p-6 max-w-3xl mx-auto bg-white shadow-lg rounded-lg">
            <h1 className="text-2xl font-bold text-gray-800 mb-4">ניהול עצים וצמחים</h1>

            <h2 className="text-xl font-semibold text-gray-700 mt-6">עצים קיימים</h2>
            <ul>
                {trees.map((tree, index) => (
                    <li key={`tree-${tree.name}-${index}`} className="border-b py-2">{tree.name}</li>
                ))}
            </ul>

            <h2 className="text-xl font-semibold text-gray-700 mt-6">צמחים קיימים</h2>
            <ul>
                {plants.map((plant, index) => (
                    <li key={`plant-${plant.name}-${index}`} className="border-b py-2">{plant.name}</li>
                ))}
            </ul>

            <h2>הוספת צמח חדש</h2>
            <input type="text" value={newPlantName} onChange={(e) => setNewPlantName(e.target.value)} placeholder="שם הצמח" />
            <button onClick={addPlant}>הוסף צמח</button>

            <h2>הוספת עץ חדש</h2>
            <input type="text" value={newTreePlantName} onChange={(e) => setNewTreePlantName(e.target.value)} placeholder="שם הצמח" />
            <button onClick={addTree}>הוסף עץ</button>

            <h2>עדכון שם צמח</h2>
            <input type="text" value={updatePlantOldName} onChange={(e) => setUpdatePlantOldName(e.target.value)} placeholder="שם ישן" />
            <input type="text" value={updatePlantNewName} onChange={(e) => setUpdatePlantNewName(e.target.value)} placeholder="שם חדש" />
            <button onClick={updatePlant}>עדכן צמח</button>

            <h2>מחיקת עץ</h2>
            <input type="text" value={deleteTreeName} onChange={(e) => setDeleteTreeName(e.target.value)} placeholder="שם העץ" />
            <button onClick={deleteTree}>מחק עץ</button>

            <h2>מחיקת צמח</h2>
            <input type="text" value={deletePlantName} onChange={(e) => setDeletePlantName(e.target.value)} placeholder="שם הצמח" />
            <button onClick={deletePlant}>מחק צמח</button>
        </div>
    );
};

export default TreeManagement;