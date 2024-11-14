<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
	import type { ControllerState } from '$lib/types/models'
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import GridForm from '$lib/components/GridForm.svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import ChartComp from './ChartComp.svelte';

    export let label: string;
    export let restPath : string;

    let controllerState : ControllerState | undefined;
    let calibrationOffsets : number[] | undefined;

    onMount(() => {
        getControllerState().then((data) => controllerState = data);
    });

    async function updateJsonRest<T>(path: string, destination : T) {
		try {
			const response = await fetch(path, {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			let json = await response.json();
            destination = json;
		} catch (error) {
			console.error('Error:', error);
		}
		return destination;
	}

    async function postJsonRest<T>(path: string, data: T) {
		try {
			const response = await fetch(path, {
				method: 'POST',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			});
			if (response.status == 200) {
				data = await response.json();
                console.log(data);
			} else {
				notifications.error('Wrong Path.', 3000);
			}
		} catch (error) {
			notifications.error('Error: ' + error, 3000);
		}
        return data;
	}

    function getControllerState() {
        return updateJsonRest(restPath, controllerState).then((data) => controllerState = data);
    }

    function postControllerState() {
        postJsonRest(restPath, controllerState).then((data) => controllerState = data);
    }

    function getCalibrationOffsets() {
        if (controllerState) controllerState.calibration.running = true;
        return updateJsonRest(restPath + '/calibration/offsets', calibrationOffsets).then((data) => calibrationOffsets = data);
    }

    let calibrationIntervalID : any;

    function startCalibration() {
        if (controllerState) controllerState.calibration.running = true;
        calibrationIntervalID = setInterval(getCalibrationOffsets, 1000);
        postJsonRest(restPath + '/calibration/start', {});
    }
    
    function stopCalibration() {
        if (controllerState) controllerState.calibration.running = false;
        clearInterval(calibrationIntervalID);
        postJsonRest(restPath + '/calibration/stop', {});
    }

    function saveCalibration() {
        stopCalibration();
        postJsonRest(restPath + '/calibration', {offsets: calibrationOffsets});
    }

</script>

<Collapsible open={true} class="shadow-lg">
	<span slot="title">{label} Controller</span>
    {#if controllerState}
        <!-- {#if controllerState.limits} -->
            <div>
                <Slider 
                    label="Target" 
                    bind:value={controllerState.target}
                    min={0} 
                    max={360} 
                    step={0.01}
                    onChange={postControllerState}
                ></Slider>
                <!-- <Slider 
                    label="Position" 
                    bind:value={controllerState.position}
                    min={0} 
                    max={360} 
                    step={0.01}
                    onChange={postControllerState}
                ></Slider> -->
                <span class="text-lg">Limits</span>
                <GridForm>
                    <Checkbox 
                        label="Enable" 
                        bind:value={controllerState.limits.enabled}
                        onChange={postControllerState}
                    ></Checkbox>
                    {#if controllerState.limits.enabled}
                    <Slider 
                        label="Begin" 
                        bind:value={controllerState.limits.begin}
                        min={0} 
                        max={360} 
                        step={0.01}
                        onChange={postControllerState}
                    ></Slider>
                    <Slider 
                        label="End" 
                        bind:value={controllerState.limits.end}
                        min={0} 
                        max={360} 
                        step={0.01}
                        onChange={postControllerState}
                    ></Slider>
                    {/if}
                </GridForm>
            </div>
        <!-- {/if} -->
        <!-- {#if controllerState.calibration} -->
            <div>
                <span class="text-lg">Calibration</span>
                <GridForm>
                    <Checkbox 
                        label="Enable" 
                        bind:value={controllerState.calibration.enabled}
                        onChange={postControllerState}
                    ></Checkbox>
                </GridForm>
                {#if controllerState.calibration.running}
                <ChartComp
                    {label}
                    data={calibrationOffsets}
                ></ChartComp>
                {/if}
                <div class="flex flex-row flex-wrap justify-between gap-x-2">
                    <button class="btn btn-primary inline-flex items-center" 
                        on:click={startCalibration}
                        ><span>Start</span></button
                    >
                    <button class="btn btn-primary inline-flex items-center" 
                        on:click={stopCalibration}
                        ><span>Stop</span></button
                    >
                    <div class="flex-grow"></div>
                    <button class="btn btn-primary inline-flex items-center" 
                        on:click={getCalibrationOffsets}
                        ><span>Get Data</span></button
                    >
                    <button class="btn btn-primary inline-flex items-center" 
                        on:click={saveCalibration}
                        ><span>Save</span></button
                    >
                </div>
            </div>
        <!-- {/if} -->
    {/if}

</Collapsible>