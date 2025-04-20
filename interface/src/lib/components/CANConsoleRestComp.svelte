<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import Text from './Text.svelte';
    import Slider from './Slider.svelte';
    import Button from './Button.svelte';
    import Checkbox from './Checkbox.svelte';
	import Terminal from '~icons/tabler/terminal'

    export let restPath: string;

    type CanMessage = {
        address: number;
        message: string;
    };

    let canMessage: CanMessage = {
        address: 0,
        message: ''
    };
    let canHistory: CanMessage[] = [];

    function postCANMessage() {
        postJsonRest(restPath + "/send", canMessage);
    }

</script>

<SettingsCard>
	<Terminal slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Can Console</span>
    <div class="alert w-full alert-success shadow-lg">
        <pre data-prefix="$"><code>npm i daisyui</code></pre>
    </div>
    <Text
        label="Send Message"
        bind:value={canMessage.message}
    />
</SettingsCard>